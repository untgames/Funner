/*
	model.cpp -- AUTOMATICALLY GENERATED; DO NOT EDIT.

  Copyright (c) 2006-2009 Primate Labs

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
  THE SOFTWARE.
*/
#include "model.h"

struct Model {
	std::string		id;
	std::string		name;
};

static Model models[] = {
	{ "ADP2,1", "Hackintosh" },
	{ "ACPI", "Hackintosh" },
	{ "iMac,1", "iMac G3" },
	{ "iMac4,1", "iMac (Early 2006)" },
	{ "iMac5,1", "iMac (Late 2006)" },
	{ "iMac6,1", "iMac (24-inch)" },
	{ "MacBook1,1", "MacBook" },
	{ "MacBook2,1", "MacBook (Late 2006)" },
	{ "MacBookPro1,1", "MacBook Pro" },
	{ "MacBookPro1,2", "MacBook Pro (17-inch)" },
	{ "MacBookPro2,1", "MacBook Pro (17-inch Core 2 Duo)" },
	{ "MacBookPro2,2", "MacBook Pro (Core 2 Duo)" },
	{ "Macmini1,1", "Mac mini (Early 2006)" },
	{ "MacPro1,1", "Mac Pro" },
	{ "PowerBook1,1", "PowerBook G3" },
	{ "PowerBook2,1", "iBook" },
	{ "PowerBook2,2", "iBook (FireWire)" },
	{ "PowerBook3,1", "PowerBook G3 (FireWire)" },
	{ "PowerBook3,2", "PowerBook G4" },
	{ "PowerBook3,3", "PowerBook G4 (Gigabit Ethernet)" },
	{ "PowerBook3,4", "PowerBook G4 (DVI)" },
	{ "PowerBook3,5", "PowerBook G4 (1 GHz/867 MHz)" },
	{ "PowerBook4,1", "iBook (Dual USB)" },
	{ "PowerBook4,2", "iBook (16 VRAM)" },
	{ "PowerBook4,3", "iBook (Opaque 16 VRAM)" },
	{ "PowerBook5,1", "PowerBook G4 (17-inch)" },
	{ "PowerBook5,2", "PowerBook G4 (15-inch FW800)" },
	{ "PowerBook5,3", "PowerBook G4 (17-inch 1.33 GHz)" },
	{ "PowerBook5,4", "PowerBook G4 (15-inch 1.5/1.33 GHz)" },
	{ "PowerBook5,5", "PowerBook G4 (17-inch 1.5 GHz)" },
	{ "PowerBook5,6", "PowerBook G4 (15-inch 1.67/1.5 GHz)" },
	{ "PowerBook5,7", "PowerBook G4 (17-inch 1.67 GHz)" },
	{ "PowerBook5,8", "PowerBook G4 (Double-Layer SD, 15-inch)" },
	{ "PowerBook5,9", "PowerBook G4 (Double-Layer SD, 17-inch)" },
	{ "PowerBook6,1", "PowerBook G4 (12-inch)" },
	{ "PowerBook6,2", "PowerBook G4 (12-inch DVI)" },
	{ "PowerBook6,3", "iBook G4" },
	{ "PowerBook6,4", "PowerBook G4 (12-inch 1.33 GHz)" },
	{ "PowerBook6,5", "iBook G4 (Early 2004)" },
	{ "PowerBook6,7", "iBook G4 (Mid 2005)" },
	{ "PowerBook6,8", "PowerBook G4 (12-inch 1.5 GHz)" },
	{ "PowerMac", "Rosetta" },
	{ "PowerMac1,1", "Power Mac G3 (Blue and White)" },
	{ "PowerMac1,2", "Power Mac G4 (PCI Graphics)" },
	{ "PowerMac2,1", "iMac (Slot Load)" },
	{ "PowerMac2,2", "iMac (Summer 2000)" },
	{ "PowerMac3,1", "Power Mac G4 (AGP Graphics)" },
	{ "PowerMac3,2", "Power Mac G4 (AGP Graphics)" },
	{ "PowerMac3,3", "Power Mac G4 (Gigabit Ethernet)" },
	{ "PowerMac3,4", "Power Mac G4 (Digital Audio)" },
	{ "PowerMac3,5", "Power Mac G4 (Quicksilver)" },
	{ "PowerMac3,6", "Power Mac G4 (Mirrored Drive Doors)" },
	{ "PowerMac4,1", "iMac G3 (Early 2001)" },
	{ "PowerMac4,2", "iMac (Flat-Panel)" },
	{ "PowerMac4,4", "eMac" },
	{ "PowerMac4,5", "iMac G4 (17-inch Flat-Panel)" },
	{ "PowerMac5,1", "Power Mac G4 Cube" },
	{ "PowerMac5,2", "Power Mac G4 Cube (Early 2001)" },
	{ "PowerMac6,1", "iMac G4 (USB 2.0)" },
	{ "PowerMac6,3", "iMac G4 (20-inch Flat-Panel)" },
	{ "PowerMac6,4", "eMac (USB 2.0)" },
	{ "PowerMac7,2", "Power Mac G5" },
	{ "PowerMac7,3", "Power Mac G5 (June 2004)" },
	{ "PowerMac8,1", "iMac G5" },
	{ "PowerMac8,2", "iMac G5 (Ambient Light Sensor)" },
	{ "PowerMac9,1", "Power Mac G5 (Late 2004)" },
	{ "PowerMac10,1", "Mac mini" },
	{ "PowerMac10,2", "Mac mini (Late 2005)" },
	{ "PowerMac11,2", "Power Mac G5 (Late 2005)" },
	{ "PowerMac12,1", "iMac G5 (iSight)" },
	{ "RackMac1,1", "Xserve" },
	{ "RackMac1,2", "Xserve (Slot Load)" },
	{ "RackMac3,1", "Xserve G5" },

	{ "", "" }
};

std::string get_model_name(std::string id)
{
	std::string	name = id;
	
	for (int i = 0; models[i].id.length() > 0; i++) {
		if (models[i].id == id) {
			name = models[i].name;
			break;
		}
	}
	return name;
}
