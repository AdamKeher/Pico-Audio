/*
Copyright 2019 AKsevenFour / Adam Keher (adam@keher.com.au)
http://www.aksevenfour.com / https://github.com/AdamKeher

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation
files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy,
modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software
is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR
IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#include "messaging.h"

Messaging::Messaging() {
  messageQueue = xQueueCreate( QUEUESIZE, sizeof( struct message * ) );   
  if(messageQueue == NULL){
    Serial.println("Error creating the messageQueue");
  }

  mainQueue = xQueueCreate( QUEUESIZE, sizeof( struct message * ) );   
  if(mainQueue == NULL){
    Serial.println("Error creating the mainQueue");
  }
}

void Messaging::sendMessage(QueueHandle_t queue, metaFunction function, void* object) {
  struct message *newMessage;
  struct message *tempMessage;


  if (function == displayMessage && ((DisplayMessage *)object)->immediate) {
    // delete existing queue items
    while (xQueueReceive(messageQueue, &tempMessage, 0))
    {
        vPortFree(tempMessage);
    }
    messageDelay = 0;
  }

  newMessage = (struct message*) pvPortMalloc(sizeof(struct message));
  newMessage->messageId = messageIdCount++;
  newMessage->function = function;
  newMessage->object = object;
  Serial.printf("MESSAGE: Send (Core %d)       | ID %04d | function %d\r\n", xPortGetCoreID(), newMessage->messageId, newMessage->function);

  xQueueSend(queue, &newMessage, 0);
}

void Messaging::display(int line, int time, bool immediate, char* messageText) {
  // TODO: Free this object!
  DisplayMessage *dmsg = (struct DisplayMessage*) pvPortMalloc(sizeof(struct DisplayMessage));
  dmsg->line = line;
  dmsg->time = time;
  dmsg->immediate = immediate;
  strcpy(dmsg->messageText, messageText);  
  sendMessage(messageQueue, displayMessage, dmsg);
}

void Messaging::loop() {
      // Process Queue
    if (uxQueueMessagesWaiting(messageQueue) > 0)
    {
      // if delay expired then process message
      if (millis() - lastMessage > messageDelay)
      {
        xQueueReceive(messageQueue, &newMessage, 0);
        Serial.printf("MESSAGE: Received (Core %d)   | ID %04d | function %d\r\n", xPortGetCoreID(), newMessage->messageId, newMessage->function);
        messageDelay = 0;
      } 
    }

    // Handle message
    if (newMessage)
    {
      Serial.printf("MESSAGE: Handle (Core %d)     | ID %04d | function %d\r\n", xPortGetCoreID(), newMessage->messageId, newMessage->function);
      if (newMessage->function == updateMetadata) {
        if (newMessage->object != NULL)        
          cb.metadata((StreamMetadata*)newMessage->object);
        Serial.printf("MESSAGE: UpdMeta (Core %d)    | ID %04d | function %d\r\n", xPortGetCoreID(), newMessage->messageId, newMessage->function);
      }
      else if (newMessage->function == displayMessage) {
        // lcd.print(0, newMessage->line, true, newMessage->messageText);
        DisplayMessage *displayMsg = (DisplayMessage *)newMessage->object;
        cb.dispdata(displayMsg->line, displayMsg->messageText);
        lastMessage = millis();
        messageDelay = displayMsg->time;
        Serial.printf("MESSAGE: disMsge (Core %d)    | ID %04d | function %d\r\n", xPortGetCoreID(), newMessage->messageId, newMessage->function);
        
        vPortFree(newMessage->object);
        newMessage->object = NULL;
      }
      else
      {
        Serial.printf("MESSAGE: Unhandled (Core: %d) | ID %04d | function %d\r\n", xPortGetCoreID(), newMessage->messageId, newMessage->function);
      }

      vPortFree(newMessage);
      newMessage = NULL;
    }
}