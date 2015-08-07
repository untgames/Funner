###################################################################################################
#Defines and constants
###################################################################################################
TARGETS := PUSH_NOTIFICATIONS.AMAZON_PUSH_NOTIFICATIONS.SOURCES PUSH_NOTIFICATIONS.AMAZON_PUSH_NOTIFICATIONS.JAVA

#Target - sources
PUSH_NOTIFICATIONS.AMAZON_PUSH_NOTIFICATIONS.SOURCES.TYPE        := static-lib
PUSH_NOTIFICATIONS.AMAZON_PUSH_NOTIFICATIONS.SOURCES.NAME        := funner.push_notifications.amazon_push_notifications
PUSH_NOTIFICATIONS.AMAZON_PUSH_NOTIFICATIONS.SOURCES.SOURCE_DIRS := sources
PUSH_NOTIFICATIONS.AMAZON_PUSH_NOTIFICATIONS.SOURCES.IMPORTS			:= compile.push_notifications.core compile.system

#Target - java
PUSH_NOTIFICATIONS.AMAZON_PUSH_NOTIFICATIONS.JAVA.TYPE        := android-jar
PUSH_NOTIFICATIONS.AMAZON_PUSH_NOTIFICATIONS.JAVA.NAME        := funner.amazon_push_notifications
PUSH_NOTIFICATIONS.AMAZON_PUSH_NOTIFICATIONS.JAVA.SOURCE_DIRS := java_sources
PUSH_NOTIFICATIONS.AMAZON_PUSH_NOTIFICATIONS.JAVA.JARS        := funner.application
PUSH_NOTIFICATIONS.AMAZON_PUSH_NOTIFICATIONS.JAVA.IMPORTS     := compile.extern.amazon_device_messaging
