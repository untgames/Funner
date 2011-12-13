# Link exports
export.link.input.iphone_driver.LIBS          := funner.input.iphone_driver
export.link.input.iphone_driver.LINK_FLAGS    := -framework UIKit -framework Foundation
export.link.input.iphone_driver.LINK_INCLUDES := IPhoneInputDriver
export.link.input.iphone_driver.IMPORTS       := link.input.low_level link.system
