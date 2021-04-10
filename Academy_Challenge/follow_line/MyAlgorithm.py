#!/usr/bin/python
#-*- coding: utf-8 -*-
import threading
import time
from datetime import datetime

import math
import cv2
import numpy as np

time_cycle = 80

class MyAlgorithm(threading.Thread):

    def __init__(self, camera, motors):
        self.camera = camera
        self.motors = motors
        self.threshold_image = np.zeros((640,360,3), np.uint8)
        self.color_image = np.zeros((640,360,3), np.uint8)
        self.stop_event = threading.Event()
        self.kill_event = threading.Event()
        self.lock = threading.Lock()
        self.threshold_image_lock = threading.Lock()
        self.color_image_lock = threading.Lock()
        threading.Thread.__init__(self, args=self.stop_event)
        self.prev_error = 0
        self.prev_time = 0
        self.sum_error = 0
    
    def getImage(self):
        self.lock.acquire()
        img = self.camera.getImage().data
        self.lock.release()
        return img

    def set_color_image (self, image):
        img  = np.copy(image)
        if len(img.shape) == 2:
          img = cv2.cvtColor(img, cv2.COLOR_GRAY2RGB)
        self.color_image_lock.acquire()
        self.color_image = img
        self.color_image_lock.release()
        
    def get_color_image (self):
        self.color_image_lock.acquire()
        img = np.copy(self.color_image)
        self.color_image_lock.release()
        return img
        
    def set_threshold_image (self, image):
        img = np.copy(image)
        if len(img.shape) == 2:
          img = cv2.cvtColor(img, cv2.COLOR_GRAY2RGB)
        self.threshold_image_lock.acquire()
        self.threshold_image = img
        self.threshold_image_lock.release()
        
    def get_threshold_image (self):
        self.threshold_image_lock.acquire()
        img  = np.copy(self.threshold_image)
        self.threshold_image_lock.release()
        return img

    def run (self):

        while (not self.kill_event.is_set()):
            start_time = datetime.now()
            if not self.stop_event.is_set():
                self.algorithm()
            finish_Time = datetime.now()
            dt = finish_Time - start_time
            ms = (dt.days * 24 * 60 * 60 + dt.seconds) * 1000 + dt.microseconds / 1000.0
            #print (ms)
            if (ms < time_cycle):
                time.sleep((time_cycle - ms) / 1000.0)

    def stop (self):
        self.stop_event.set()

    def play (self):
        if self.is_alive():
            self.stop_event.clear()
        else:
            self.start()

    def kill (self):
        self.kill_event.set()

    def algorithm(self):
        def line_detection(img):
            # Convert to Grayscale or HSV
            hsv_img = cv2.cvtColor(img, cv2.COLOR_RGB2HSV)
            lower_bound = (0,0,0)
            higher_bound = (150,150,150)
            mask = cv2.inRange(hsv_img, lower_bound, higher_bound)
            mask = 255 - mask
            return mask

        def crop_image(img):
            # as we will have PID controller, so getting out of the track and many corner cases for the cropped image that will not work, will not happen
            h, w = img.shape[:2]
            new_img = img[h-100:h, :]
            # print(len(new_img), len(new_img[0]))
            new_img[0:5] = 0
            new_img[-6:-1][:] = 0
            new_img[-1][:] = 0
            for i in range(100):
                new_img[i][0:20] = 0
                new_img[i][550:640] = 0
            # new_img[i][-1] = 0
            # new_img[:][0:5] = 0
            # new_img[:][-6:-1] = 0
            return new_img

        def get_set_point(img):
            _, contours, _ = cv2.findContours(img, cv2.RETR_TREE, cv2.CHAIN_APPROX_SIMPLE)
            if(len(contours) == 0):
                return [320,50],img
            cnt = contours[-1]
            rect = cv2.minAreaRect(cnt)
            box = cv2.boxPoints(rect)
            box = np.int0(box)
            M = cv2.moments(cnt)
            if(M["m00"] == 0):
                return [320,50],img

            cX = int(M["m10"] / M["m00"])
            cY = int(M["m01"] / M["m00"])
            set_point = (cX,cY)
            set_point_image = cv2.circle(img, set_point, 5, 0, -1)
            return set_point, set_point_image

        def control(set_point):
            kp,ki,kd = 0.045,0.08,0.05
            error_vector = [320 - set_point[0], 50 - set_point[1]]
            error = math.sqrt(abs(error_vector[0]))
            if(error_vector[0] < 0):
                error *= -1 
            print("error", error)
            # error = min(max(error,-10),10)
            print("error+",error)
            dt = (time.time()-self.prev_time)
            # print("dt", dt)
            u_P = kp*error
            u_D = kd*(error-self.prev_error)
            u_I = ki*(self.sum_error)*dt
            print("P: {:} \t I: {:} \t D: {:}".format(u_P, u_I, u_D))
            u = u_P + u_I + u_D
            print("u",u)
            # u = min(max(u,-5),5)
            # print("u+",u)
            self.motors.sendV(2)
            self.motors.sendW(u)

            self.prev_error = error
            self.sum_error += error
            self.prev_time = time.time()



        #GETTING THE IMAGES
        image = self.getImage()

        # Add your code here
        print("Runing")

        binary_image = line_detection(image)
        cropped_binary_image = crop_image(binary_image)
        set_point, set_point_image = get_set_point(cropped_binary_image)
        print("set_point",set_point)
        control(set_point)
        #EXAMPLE OF HOW TO SEND INFORMATION TO THE ROBOT ACTUATORS

        #SHOW THE FILTERED IMAGE ON THE GUI
        self.set_threshold_image(set_point_image)
