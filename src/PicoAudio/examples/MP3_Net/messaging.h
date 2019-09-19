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
#ifndef MESSAGING_H
#define MESSAGING_H

#include <Arduino.h>
#include "metadata.h"

#define QUEUESIZE 10

enum metaFunction
{
  updateMetadata,
  volumeUp,
  volumeDown,
  displayMessage,
  nextTrack,
  prevTrack
};

struct DisplayMessage {
  int line;
  int time;
  bool immediate = false;
  char messageText[64];
};

struct message {
  uint32_t messageId;
  metaFunction function;
  void *object;
};

class MessagingEvents
{
    public:
        MessagingEvents() { ClearCBs(); }
        void ClearCBs() { metaFn = NULL; };

        typedef void (*metaCBFn)(void *cbData, StreamMetadata *metadata);
        bool RegisterMetadataCB(metaCBFn f, void *cbData) { metaFn = f; metaData = cbData; return true; }
        inline void metadata(StreamMetadata *metadata) { if (metaFn) metaFn(metaData, metadata); }

        typedef void (*dispCBFn)(void *cbData, int line, char *text);
        bool RegisterDisplayCB(dispCBFn f, void *cbData) { dispFn = f; dispData = cbData; return true; }
        inline void dispdata(int line, char *text) { if (dispFn) dispFn(dispData, line, text); }
    private:
        metaCBFn metaFn;
        void *metaData;

        dispCBFn dispFn;
        void *dispData;
};

class Messaging
{
    public:
      Messaging();
      // void sendMessage(QueueHandle_t queue, metaFunction function, int line, int time, char *messageText, bool immediate = false, uint16_t data = 0, void* object = NULL);
      void sendMessage(QueueHandle_t queue, metaFunction function, void* object = NULL);      
      void display(int line, int time, bool immediate, char *messageText);

      bool RegisterMetadataCB(MessagingEvents::metaCBFn fn, void *data) { return cb.RegisterMetadataCB(fn, data); }
      bool RegisterDisplayCB(MessagingEvents::dispCBFn fn, void *data) { return cb.RegisterDisplayCB(fn, data); }

      QueueHandle_t messageQueue;
      QueueHandle_t mainQueue;      
      int messageDelay;

      void loop();

    private:
        MessagingEvents cb;
        int messageIdCount;
        struct message *newMessage = NULL;
        uint32_t lastMessage = 0;
};

#endif // MESSAGING_H