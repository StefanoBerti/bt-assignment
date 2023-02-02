/*
 *   Copyright (c) 2022 Andrea Rosasco
 *   All rights reserved.

 *   Permission is hereby granted, free of charge, to any person obtaining a copy
 *   of this software and associated documentation files (the "Software"), to deal
 *   in the Software without restriction, including without limitation the rights
 *   to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 *   copies of the Software, and to permit persons to whom the Software is
 *   furnished to do so, subject to the following conditions:

 *   The above copyright notice and this permission notice shall be included in all
 *   copies or substantial portions of the Software.

 *   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *   IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 *   FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 *   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 *   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 *   OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 *   SOFTWARE.
 */

/******************************************************************************
 *                                                                            *
 * Copyright (C) 2020 Fondazione Istituto Italiano di Tecnologia (IIT)        *
 * All Rights Reserved.                                                       *
 *                                                                            *
 ******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#include <yarp/os/Network.h>
#include <yarp/os/RpcServer.h>

#include <ActionRecognitionInterface.h>
#include <atomic>


const int TYPE_DISTANCE_POSE = 1;
const int TYPE_DISTANCE = 2;
const int TYPE_POSE = 3;
const int TYPE_NONE = 4;


#define  BUFF_SIZE   8+4+1

typedef struct {
	long  data_type;
	unsigned char  data_buff[BUFF_SIZE];
} t_data;


class ActionRecognition : public ActionRecognitionInterface
{
public:
    ActionRecognition() = default;
    bool open()
    {

        this->yarp().attachAsServer(server_port);
        if (!server_port.open("/Components/ActionRecognition")) {
            yError("Could not open ");
            return false;
        }

        if ( -1 == (this->msqid = msgget( (key_t)5678, IPC_CREAT | 0666)))
        {
            perror( "msgget() failed");
            exit( 1);
        }

        return true;
    }

    void close()
    {
        server_port.close();
    }


    std::int16_t get_action()
    {
        this->read_queue();
        return this->action;
    }

    bool is_focused()
    {
        this->read_queue();
        return this->focus;
    }

    double get_distance()
    {
        this->read_queue();
        return this->distance;
    }

private:
    yarp::os::RpcServer server_port;
    double distance;
    std::int16_t action;
    bool focus;
    long msg_type;
    int msqid;

    void read_queue()
    {
	    t_data   data;

        int success = msgrcv( this->msqid, &data, sizeof( t_data) - sizeof( long), 0, IPC_NOWAIT) != -1;
        printf("%d", success);
        if (success) {
            printf("Reading message of type %ld", data.data_type);
            this->msg_type = data.data_type; }
        else if ( !success && errno == ENOMSG) {
            printf("Empty queue");
            this->distance = -1.;
            this->action = -1;
            this->focus = false;
            return;}
		else {
		    this->msg_type = TYPE_NONE;
		    perror( "msgrcv() failed");
			exit( 1);
		}

        memcpy(&this->action, data.data_buff, 2);
        memcpy(&this->distance, data.data_buff+2, 8);
        memcpy(&this->focus, data.data_buff+10, 1);
    }
};

int main()
{
    yarp::os::Network yarp;

    ActionRecognition actionRecognition;
    if (!actionRecognition.open()) {
        return 1;
    }

    while (true) {
        yInfo("Server running happily");
        yarp::os::Time::delay(10);
    }

    actionRecognition.close();

    return 0;
}
