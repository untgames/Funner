###################################################################################################
#Defines and constants
###################################################################################################
TARGETS := PUSH_NOTIFICATIONS_COMPONENTS

#Target - package
PUSH_NOTIFICATIONS_COMPONENTS.TYPE               := package
PUSH_NOTIFICATIONS_COMPONENTS.COMPONENTS         := core
PUSH_NOTIFICATIONS_COMPONENTS.android.COMPONENTS := google_push_notifications amazon_push_notifications
PUSH_NOTIFICATIONS_COMPONENTS.iphone.COMPONENTS  := ios_push_notifications

