//
// This file is part of VirtualC64 - A cycle accurate Commodore 64 emulator
//
// Copyright (C) Dirk W. Hoffmann. www.dirkwhoffmann.de
// Licensed under the GNU General Public License v3
//
// See https://www.gnu.org for license information
//

import Foundation

class VirtualKeyboardWindow : NSWindow {
    
    func respondToEvents() {
        
        track()
        DispatchQueue.main.async {
            self.makeFirstResponder(self)
        }
    }
    
    override func awakeFromNib() {
        
        respondToEvents()
    }
    
    override func keyDown(with event: NSEvent) {
        
        track()
        myController?.metalScreen.keyDown(with: event)
        let controller = delegate as! VirtualKeyboardController
        controller.refresh()
    }
    
    override func keyUp(with event: NSEvent) {
        
        track()
        myController?.metalScreen.keyUp(with: event)
        let controller = delegate as! VirtualKeyboardController
        controller.refresh()
    }
    
    override func flagsChanged(with event: NSEvent) {
        
        track()
        myController?.metalScreen.flagsChanged(with: event)
        let controller = delegate as! VirtualKeyboardController
        controller.refresh()
    }
}

class VirtualKeyboardController : UserDialogController, NSWindowDelegate
{
    // Amiga model and language
    var model : AmigaModel = A500
    var language : Language = .us
    
    // Array holding a reference to the view of each key
    var keyView = Array(repeating: nil as NSButton?, count: 128)

    // Image cache
    var keyImage = Array(repeating: nil as NSImage?, count: 128)

    // Image cache for keys that are currently pressed
    var pressedKeyImage = Array(repeating: nil as NSImage?, count: 128)

    /* Indicates if the window should close when a key is pressed.
     * If the virtual keyboard is opened as a sheet, this variable is set to
     * true. If it is opened as a seperate window, it is set to false.
     */
    var autoClose = true
    
    static func make() -> VirtualKeyboardController? {

        guard let config = myController?.amiga.config() else { return nil }

        let language = Language(rawValue: config.localization) ?? .us
        let ansi = language == .us
        
        var xibName = ""
        
        if config.model == A1000 {
            xibName = ansi ? "A1000ANSI" : "A1000ISO"
        } else {
            xibName = ansi ? "A500ANSI" : "A500ISO"
        }
      
        let controller = VirtualKeyboardController.init(windowNibName: xibName)
        
        controller.model = config.model
        controller.language = language
        return controller
    }
    
    func showWindow() {
        
        autoClose = false
        showWindow(self)
    }
    
    override func windowDidLoad() {
        
        track()
        
        // Setup key references
        for tag in 0 ... 127 {
            keyView[tag] = window!.contentView!.viewWithTag(tag) as? NSButton
            /*
            if let buttonCell = keyView[tag]?.cell as? NSButtonCell {
                buttonCell.highlightsBy = []
            }
            */
        }

        updateImageCache()
    }
    
    func windowWillClose(_ notification: Notification) {
    
        track()
        // releaseSpecialKeys()
    }
    
    func windowDidBecomeMain(_ notification: Notification) {
        
        track()
        refresh()
    }
    
    override func refresh() {
        
        track()
        
        guard let keyboard = amigaProxy?.keyboard else { return }
        
        for keycode in 0 ... 127 {
            
            if keyboard.keyIsPressed(keycode) {
                keyView[keycode]?.image = pressedKeyImage[keycode]
            } else {
                keyView[keycode]?.image = keyImage[keycode]
            }
        }
    }
    
    func updateImageCache() {
        
        for keycode in 0 ... 127 {
            let key = AmigaKey.init(keyCode: keycode)
            if let image = key.image(model: model, language: language) {
                keyImage[keycode] = image
                // track("\(key)")
                pressedKeyImage[keycode] = image.copy() as? NSImage
                pressedKeyImage[keycode]?.pressed()
            }
        }
    }
    
    func pressKey(keyCode: Int) {
        
        guard let keyboard = amigaProxy?.keyboard else { return }

        keyboard.pressKey(keyCode)
        refresh()
        
        DispatchQueue.main.async {
            
            usleep(useconds_t(100000))
            amigaProxy?.keyboard.releaseAllKeys()
            self.refresh()
        }
        
        if (autoClose) {
            cancelAction(self)
        }
    }
    
    func holdKey(keyCode: Int) {
        
        guard let keyboard = amigaProxy?.keyboard else { return }
        
        keyboard.pressKey(keyCode)
        refresh()
    }
        
    @IBAction func pressVirtualKey(_ sender: NSButton!) {
        
        // not used at the moment
    }
    
    override func mouseDown(with event: NSEvent) {
                
        // Close window if the user clicked inside the unused area
        track()
        if (autoClose) {
            cancelAction(self)
        }
    }
}

/* Subclass of NSButton for the keys in the virtual keyboard.
 */
class Keycap : NSButton {
    
    override func mouseDown(with event: NSEvent) {
        
        if let controller = window?.delegate as? VirtualKeyboardController {
            
            controller.pressKey(keyCode: self.tag)
        }
    }
    
    override func rightMouseDown(with event: NSEvent) {
    
        if let controller = window?.delegate as? VirtualKeyboardController {
            
            controller.holdKey(keyCode: self.tag)
        }
    }
}
