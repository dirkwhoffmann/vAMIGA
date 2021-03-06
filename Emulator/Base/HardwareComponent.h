// -----------------------------------------------------------------------------
// This file is part of vAmiga
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
// -----------------------------------------------------------------------------

#pragma once

#include "HardwareComponentTypes.h"
#include "AmigaObject.h"
#include "Serialization.h"
#include "Concurrency.h"

#include <vector>
#include <iostream>

/* This class defines the base functionality of all hardware components. It
 * comprises functions for initializing, configuring, and serializing the
 * emulator, as well as functions for powering up and down, running and
 * pausing. Furthermore, a 'synchronized' macro is provided to prevent mutual
 * execution of certain code components.
 */

#define synchronized \
for (util::AutoMutex _am(mutex); _am.active; _am.active = false)

namespace dump {
enum Category : usize {
    
    Config    = 0b000000001,
    State     = 0b000000010,
    Registers = 0b000000100,
    Events    = 0b000001000,
    Checksums = 0b000010000,
    Dma       = 0b000100000,
    BankMap   = 0b001000000,
    List1     = 0b010000000,
    List2     = 0b100000000,
};
}

class HardwareComponent : public AmigaObject {
    
    friend class Amiga;
    
public:
    
    // Sub components
    std::vector<HardwareComponent *> subComponents;
    
protected:

    /* Indicates if the emulator should be executed in warp mode. To speed up
     * emulation (e.g., during disk accesses), the virtual hardware may be put
     * into warp mode. In this mode, the emulation thread is no longer paused
     * to match the target frequency and runs as fast as possible.
     */
    bool warpMode = false;
    
    /* Indicates if the emulator should be executed in debug mode. Debug mode
     * is enabled when the GUI debugger is opend and disabled when the GUI
     * debugger is closed. In debug mode, several time-consuming tasks are
     * performed that are usually left out. E.g., the CPU checks for
     * breakpoints and records the executed instruction in it's trace buffer.
     */
    bool debugMode = false;
            
    /* Mutex for implementing the 'synchronized' macro. The macro can be used
     * to prevent multiple threads to enter the same code block. It mimics the
     * behaviour of the well known Java construct 'synchronized(this) { }'.
     */
    util::ReentrantMutex mutex;

        
    //
    // Initializing
    //
    
public:
    
    virtual ~HardwareComponent();
    
    /* Initializes the component and it's subcomponents. The initialization
     * procedure puts every components back into it's initial state.
     */
    void initialize();
    virtual void _initialize() = 0;
    
    /* Resets the component and it's subcomponents. Two reset modes are
     * distinguished:
     *
     *     hard: A hard reset restores the initial state. It resets the Amiga
     *           from an emulator point of view.
     *
     *     soft: A soft reset emulates a reset inside the virtual Amiga. It is
     *           used to emulate the RESET instruction of the CPU.
     */
    void reset(bool hard);
    virtual void _reset(bool hard) = 0;
    
    
    //
    // Configuring
    //
    
    /* Configures the component and it's subcomponents. This function
     * distributes a configuration request to all subcomponents by calling
     * setConfigItem(). The function returns true iff the current configuration
     * has changed.
     */
    bool configure(Option option, i64 value) throws;
    bool configure(Option option, long id, i64 value) throws;
    
    /* Requests the change of a single configuration item. Each sub-component
     * checks if it is responsible for the requested configuration item. If
     * yes, it changes the internal state. If no, it ignores the request.
     * The function returns true iff the current configuration has changed.
     */
    virtual bool setConfigItem(Option option, i64 value) throws { return false; }
    virtual bool setConfigItem(Option option, long id, i64 value) throws { return false; }
    
        
    //
    // Analyzing
    //
    
    /* Collects information about the component and it's subcomponents. Many
     * components contain an info variable of a class specific type (e.g.,
     * CPUInfo, MemoryInfo, ...). These variables contain the information shown
     * in the GUI's inspector window and are updated by calling this function.
     * Note: Because this function accesses the internal emulator state with
     * many non-atomic operations, it must not be called on a running emulator.
     * To carry out inspections while the emulator is running, set up an
     * inspection target via Amiga::setInspectionTarget().
     */
    void inspect();
    virtual void _inspect() { }
    
    /* Base method for building the class specific getInfo() methods. When the
     * emulator is running, the result of the most recent inspection is
     * returned. If the emulator isn't running, the function first updates the
     * cached values in order to return up-to-date results.
     */
    template<class T> T getInfo(T &cachedValues) {
        
        if (!isRunning()) inspect();
        
        T result;
        synchronized { result = cachedValues; }
        return result;
    }
    
    /* Prints debug information about this component. The additional 'flags'
     * parameter is a bit field which can be used to limit the displayed
     * information to certain categories.
     */
    void dump(dump::Category category, std::ostream& ss) const;
    void dump(dump::Category category) const;
    void dump(std::ostream& ss) const;
    void dump() const;
    virtual void _dump(dump::Category category, std::ostream& ss) const { };

    
    //
    // Serializing
    //
    
    // Returns the size of the internal state in bytes
    isize size();
    virtual isize _size() = 0;
    
    // Loads the internal state from a memory buffer
    isize load(const u8 *buffer);
    virtual isize _load(const u8 *buffer) = 0;
    
    // Saves the internal state to a memory buffer
    isize save(u8 *buffer);
    virtual isize _save(u8 *buffer) = 0;
    
    /* Delegation methods called inside load() or save(). Some components
     * override these methods to add custom behavior if not all elements can be
     * processed by the default implementation.
     */
    virtual isize willLoadFromBuffer(const u8 *buffer) { return 0; }
    virtual isize didLoadFromBuffer(const u8 *buffer) { return 0; }
    virtual isize willSaveToBuffer(u8 *buffer) const {return 0; }
    virtual isize didSaveToBuffer(u8 *buffer) const { return 0; }
    
    
    //
    // Controlling
    //
    
public:
    
    /* State model. At any time, a component is in one of three states:
     *
     *        Off: The Amiga is turned off
     *     Paused: The Amiga is turned on, but there is no emulator thread
     *    Running: The Amiga is turned on and the emulator thread running
     *
     *          -----------------------------------------------
     *         |                     run()                     |
     *         |                                               V
     *     ---------   powerOn()   ---------     run()     ---------
     *    |   Off   |------------>| Paused  |------------>| Running |
     *    |         |<------------|         |<------------|         |
     *     ---------   powerOff()  ---------    pause()    ---------
     *         ^                                               |
     *         |                  powerOff()                   |
     *          -----------------------------------------------
     *
     *     isPoweredOff()         isPaused()          isRunning()
     * |-------------------||-------------------||-------------------|
     *                      |----------------------------------------|
     *                                     isPoweredOn()
     *
     * Additional component flags: warp (on / off), debug (on / off)
     */
    
    virtual bool isPoweredOff() const = 0;
    virtual bool isPoweredOn() const = 0;
    virtual bool isPaused() const = 0;
    virtual bool isRunning() const = 0;
    
private:
    
    /* powerOn() powers the component on
     *
     * current   | next      | action
     * ------------------------------------------------------------------------
     * off       | paused    | _powerOn() on each subcomponent
     * paused    | paused    | none
     * running   | running   | none
     */
    void powerOn();
    virtual void _powerOn() { };
    
    /* powerOff() powers the component off
     *
     * current   | next      | action
     * ------------------------------------------------------------------------
     * off       | off       | none
     * paused    | off       | _powerOff() on each subcomponent
     * running   | off       | pause(), _powerOff() on each subcomponent
     */
    void powerOff();
    virtual void _powerOff() { }
    
    /* run() puts the component in 'running' state
     *
     * current   | next      | action
     * ------------------------------------------------------------------------
     * off       | running   | powerOn(), _run() on each subcomponent
     * paused    | running   | _run() on each subcomponent
     * running   | running   | none
     */
    void run();
    virtual void _run() { }
    
    /* pause() puts the component in 'paused' state
     *
     * current   | next      | action
     * ------------------------------------------------------------------------
     * off       | off       | none
     * paused    | paused    | none
     * running   | paused    | _pause() on each subcomponent
     */
    void pause();
    virtual void _pause() { };
        
    // Switches warp mode on or off
    void warpOn();
    virtual void _warpOn() { };

    void warpOff();
    virtual void _warpOff() { };

    // Switches debug mode on or off
    void debugOn();
    virtual void _debugOn() { };

    void debugOff();
    virtual void _debugOff() { };
};

//
// Standard implementations of _reset, _load, and _save
//

#define COMPUTE_SNAPSHOT_SIZE \
util::SerCounter counter; \
applyToPersistentItems(counter); \
applyToHardResetItems(counter); \
applyToResetItems(counter); \
return counter.count;

#define RESET_SNAPSHOT_ITEMS(hard) \
{ \
util::SerResetter resetter; \
if (hard) applyToHardResetItems(resetter); \
applyToResetItems(resetter); \
debug(SNP_DEBUG, "Resetted (%s)\n", hard ? "hard" : "soft"); \
}

#define LOAD_SNAPSHOT_ITEMS \
{ \
util::SerReader reader(buffer); \
applyToPersistentItems(reader); \
applyToHardResetItems(reader); \
applyToResetItems(reader); \
debug(SNP_DEBUG, "Recreated from %zu bytes\n", reader.ptr - buffer); \
return (isize)(reader.ptr - buffer); \
}

#define SAVE_SNAPSHOT_ITEMS \
{ \
util::SerWriter writer(buffer); \
applyToPersistentItems(writer); \
applyToHardResetItems(writer); \
applyToResetItems(writer); \
debug(SNP_DEBUG, "Serialized to %zu bytes\n", writer.ptr - buffer); \
return (isize)(writer.ptr - buffer); \
}
