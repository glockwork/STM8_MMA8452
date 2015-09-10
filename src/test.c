#include <iostm8s003f3.h>
#include <stdio.h>
#include "MMA8452.h"
#define RINGBUFFERSIZE 38     //缓存区大小
#define minValue 3200         //运动开始阈值
#define maxValue 5000         //运动最大标志阈值

//缓冲区指针向下移动
#define nextPoint(point) if(point == &RingBuffer[RINGBUFFERSIZE - 1])point = RingBuffer;else point++;
//缓冲区指针向上移动
#define prevPoint(point) if(point == RingBuffer)point = &RingBuffer[RINGBUFFERSIZE - 1];else point--;
//取绝对值
#define absInt(t) t<0?(-t):t
#define absChar(t) if(t&0x80)t=255-t;;
//开启定时器
#define TIM1ON TIM1_CR1_CEN = 1;timeOvarFlag = 0;



extern char BUF[3];
struct Data{
    char x;
    char y;
    char z;
    unsigned int absForce;  
};
struct Data RingBuffer[RINGBUFFERSIZE];
static struct Data *HeadPoint = RingBuffer;    //缓冲区头指针
static struct Data *MotionPoint = NULL;        //运动开始标志指针
static struct Data *EndPoint = NULL;           //缓冲区逻辑尾指针标志指针
static struct Data StaticData;                 //静止点
static struct Data minAbsForce;
static struct Data maxAbsForce;
static struct Data min2AbsForce;
static unsigned char timeOvarFlag = 0;

void prevsPoint(struct Data **tempPoint,unsigned char len){
    unsigned char tempLen = *tempPoint - RingBuffer;
    
    if(tempLen > len)
        *tempPoint -= len;
    else{
        tempLen = len - tempLen;
        *tempPoint = &RingBuffer[RINGBUFFERSIZE-1] - tempLen;
    }
}

void test(){
    struct Data *te;
    
    char dd = 0;
    
    te = &RingBuffer[10];
    
    prevsPoint(&te,2);
    
    dd = te - RingBuffer;
    
}


#pragma vector = TIM1_OVR_UIF_vector 
__interrupt void TIM1_(){
    TIM1_SR1_UIF = 0;
    timeOvarFlag ++;
    if(timeOvarFlag == 2)
        TIM1_CR1_CEN = 0;
}
#pragma vector = EXTI3_vector
__interrupt void PD_(){
    static char temp = 0;
    static struct Data data[3];
    unsigned char swpState;
    unsigned char locat;
    static struct Data swp;
    readI2C(SlaveAddress,0x01,BUF,3);
    switch(temp){
      case 0:
        data[0].x = BUF[0];
        data[0].y = BUF[1];
        data[0].z = BUF[2];
        absChar(BUF[0]);
        absChar(BUF[1]);
        absChar(BUF[2]);
        data[0].absForce = BUF[0]*BUF[0]+BUF[1]*BUF[1]+BUF[2]*BUF[2];
        temp = 1;
        break;
      case 1:
        data[1].x = BUF[0];
        data[1].y = BUF[1];
        data[1].z = BUF[2];
        absChar(BUF[0]);
        absChar(BUF[1]);
        absChar(BUF[2]);
        data[1].absForce = BUF[0]*BUF[0]+BUF[1]*BUF[1]+BUF[2]*BUF[2];
        temp = 2;
        break;
      case 2:
        data[2].x = BUF[0];
        data[2].y = BUF[1];
        data[2].z = BUF[2];
        absChar(BUF[0]);
        absChar(BUF[1]);
        absChar(BUF[2]);
        data[2].absForce = BUF[0]*BUF[0]+BUF[1]*BUF[1]+BUF[2]*BUF[2];
        temp = 0;
        swpState = 1;
        while(swpState){
            swpState = 0;
            for(locat = 0;locat < 2;locat ++){
                if(data[locat].absForce > data[locat+1].absForce){
                    swp = data[locat];
                    data[locat] = data[locat+1];
                    data[locat+1] = swp;
                    swpState = 1;
                }
            }
        }
        //        printf("%u ",data[1].absForce);   //打印滤波后的值
        *HeadPoint = data[1];
        if(MotionPoint == NULL && HeadPoint->absForce < minValue){
            MotionPoint = HeadPoint;
            TIM1ON
        }
        if(EndPoint == HeadPoint){
            EndPoint = NULL;
        }
        
        
        nextPoint(HeadPoint)
    }
}

void dome(){
    static unsigned char switchState = 0;          //switch变量
    //case1 和 case2使用
    static unsigned char time = 0;
    if(MotionPoint != NULL){
        switch(switchState){
          case 0:{
              unsigned char distance;                            //距离
              struct Data *tempPoint = MotionPoint;              //临时处理指针
              unsigned char tempLen;
              unsigned char swpState = 1;
              struct Data *swpPoint,*swpPoint2,swpPointData;
              unsigned char time;
              do{
                  if(tempPoint->absForce > maxValue){
                      switchState = 4;
                      goto A;
                  }
                  
                  //临时处理指针向前移动
                  prevPoint(tempPoint)
                      
                      //计算处理距离
                      if(EndPoint){   
                          if(tempPoint < EndPoint){
                              distance = tempPoint - RingBuffer;
                              distance +=  &RingBuffer[RINGBUFFERSIZE-1] - EndPoint;
                          } else {
                              distance = tempPoint - EndPoint;
                          }
                          
                      } else {
                          if(tempPoint < HeadPoint){
                              distance = tempPoint - RingBuffer;
                              distance +=  &RingBuffer[RINGBUFFERSIZE-1] - HeadPoint;
                          }
                          else{
                              distance = tempPoint - HeadPoint;
                          }
                      }
                  
                  
              }while(distance > 2);
              tempPoint = MotionPoint;
              time = 0;
              do{
                  //指针向前移动2个
                  tempLen = tempPoint - RingBuffer;
                  
                  if(tempLen >= 2)
                      tempPoint -= 2;
                  else{
                      tempLen = 2 - tempLen;
                      tempPoint = &RingBuffer[RINGBUFFERSIZE-1] - tempLen;
                  }
                  
                  swpState = 1;
                  
                  while(swpState){
                      swpState = 0;
                      swpPoint = MotionPoint;
                      while(swpPoint!=tempPoint){
                          swpPoint2 = swpPoint;
                          prevPoint(swpPoint);
                          if(swpPoint->absForce > swpPoint2->absForce){
                              swpPointData = *swpPoint2;
                              *swpPoint2 = *swpPoint;
                              *swpPoint = swpPointData;
                              swpState = 1;
                          }
                          
                      }
                  }
                  
                  
                  
                  //计算处理距离
                  if(EndPoint){   
                      if(tempPoint < EndPoint){
                          distance = tempPoint - RingBuffer;
                          distance +=  &RingBuffer[RINGBUFFERSIZE-1] - EndPoint;
                      } else {
                          distance = tempPoint - EndPoint;
                      }
                      
                  } else {
                      if(tempPoint < HeadPoint){
                          distance = tempPoint - RingBuffer;
                          distance +=  &RingBuffer[RINGBUFFERSIZE-1] - HeadPoint;
                      }
                      else{
                          distance = tempPoint - HeadPoint;
                      }
                  }
                  
                  
                  //                打印处理过程
                  //printf("距离:%d \n\r\n",distance);
                  
                  time++;
              }while(distance > 5);
              //printf("处理次数:%d \n\r\n",time);
              /*tempPoint = MotionPoint;
              prevPoint(tempPoint)
              while(tempPoint!=MotionPoint){
              prevPoint(tempPoint);
              printf("下标:%d 值:%u \n\r\n",tempPoint-RingBuffer,tempPoint->absForce);
              
          }*/
              
              //asm("sim");
              //保存静止点
              tempLen = MotionPoint - RingBuffer;
              //printf("MotionPoint:%d 处理次数:time \n\r\n",MotionPoint-RingBuffer,);
              if(tempLen >=  time){
                  tempPoint = MotionPoint - time;
                  //printf("a静止值:%u 位置:%d \n\r\n",tempPoint->absForce,tempLen - time);
                  /*if(StaticData.absForce < 3600){
                  printf("超过临界值 \n\r\n");
                  for(tempPoint = MotionPoint - time ; tempPoint != MotionPoint ;) {
                  printf("静止值:%u 位置:%d",tempPoint->absForce,tempPoint-RingBuffer);
                  nextPoint(tempPoint);
              }
                  
              }*/
              }else{
                  
                  tempLen =  time - tempLen;
                  
                  tempPoint = &RingBuffer[RINGBUFFERSIZE-1] -  tempLen;
                  
                  //printf("b静止值:%u 位置:%d \n\r\n",tempPoint->absForce,(&RingBuffer[RINGBUFFERSIZE-1] -  tempPoint));
                  /*if(StaticData.absForce < 3600){
                  printf("超过临界值 \n\r\n");
                  for(tempPoint = &RingBuffer[RINGBUFFERSIZE-1] -  tempLen ; tempPoint != MotionPoint ;) {
                  printf("静止值:%u 位置:%d",tempPoint->absForce,tempPoint-RingBuffer)
                  nextPoint(tempPoint);
              }
              }
                  */
              }
              //printf("aa静止值:%u \n\r\n",tempPoint->absForce);
              if(tempPoint->absForce < 3700){
                  //printf("超过临界值 \n\r\n");
                  while(tempPoint != MotionPoint){
                      if(tempPoint->absForce > 3800){
                          if(tempPoint < MotionPoint){
                              distance = MotionPoint - tempPoint;
                              tempPoint += distance/2;
                          }
                          else{
                              distance = &RingBuffer[RINGBUFFERSIZE-1] - tempPoint;
                              distance +=  MotionPoint - RingBuffer;
                              distance /= 2;
                              if(distance < &RingBuffer[RINGBUFFERSIZE-1] - tempPoint){
                                  tempPoint += distance;
                              }
                              else{
                                  distance -= &RingBuffer[RINGBUFFERSIZE-1] - tempPoint;
                                  tempPoint = RingBuffer + distance;
                              }
                          }
                          goto tem;
                      }
                      nextPoint(tempPoint);
                  }
            
                  tempPoint->absForce = 3800;
                  
              }
            tem:
              //printf("ab静止值:%u ",StaticData.absForce);
              StaticData = *tempPoint;
              printf("静止值:%u \n\r\n",StaticData.absForce);
              //               tempPoint = MotionPoint;
              //              prevPoint(tempPoint)
              //                while(tempPoint!=MotionPoint){
              //                  prevPoint(tempPoint);
              //                  printf("下标:%d 值:%u \n\r\n",tempPoint-RingBuffer,tempPoint->absForce);
              //                  
              //                }   
              //                asm("rim");  
              
              //printf("处理次数 %d \n\r\n",time);
              //            打印处理结果
              
              switchState = 1;
              //StaticData = *staticFlagPoint;
              
              //为case1的参数做初始化
              minAbsForce.absForce = minValue;
              maxAbsForce.absForce = maxValue;
              min2AbsForce.absForce = minValue*1.5;
          }
          case 1:
            while(MotionPoint != HeadPoint){
                if(MotionPoint->absForce < minAbsForce.absForce){
                    minAbsForce = *MotionPoint;
                }else if(MotionPoint->absForce > maxAbsForce.absForce ){
                    printf("minAbsForce:%u \n\r\n",minAbsForce.absForce);
                    maxAbsForce = *MotionPoint;
                    switchState = 2;
                    goto B;
                }
                
                //等待静止点
                if(absInt(StaticData.absForce-MotionPoint->absForce) < 300){
                    time++;
                    if(time == 3){
                        time = 0;
                        printf("minAbsForce:%u \n\r\n",minAbsForce.absForce);
                        //printf("向下运动 by Case1 \n\r\n");
                        switchState = 5;
                        TIM1ON
                            break;
                    }     
                }
                else{
                    time = 0;
                }
                nextPoint(MotionPoint)
            }
            printf("跳出循环");
            break;
          case 2:
          B:           
            while(MotionPoint != HeadPoint){
                if(MotionPoint->absForce > maxAbsForce.absForce ){
                    maxAbsForce = *MotionPoint;
                }else if(MotionPoint->absForce < min2AbsForce.absForce){
                    printf("maxAbsForce:%u \n\r\n",maxAbsForce.absForce);
                    min2AbsForce = *MotionPoint;
                    switchState = 3;
                    goto C;
                    
                }
                //printf("MotionPoint:%u \n\r\n",MotionPoint->absForce);  
                //等待静止点
                if(absInt(StaticData.absForce-MotionPoint->absForce) < 300){
                    time++;
                    if(time == 3){
                        time = 0;
                        printf("maxAbsForce:%u \n\r\n",maxAbsForce.absForce);
                        //printf("向下运动 by Case2 \n\r\n");
                        TIM1ON
                            switchState = 5;
                        break;
                    }     
                }
                else{
                    time = 0;
                }
                
                nextPoint(MotionPoint)
            }
            break;
          case 3:
          C:           
          while(MotionPoint != HeadPoint){
              if(MotionPoint->absForce < min2AbsForce.absForce){
                  min2AbsForce = *MotionPoint;
              }
              //printf("MotionPoint:%u \n\r\n",MotionPoint->absForce);  
              //等待静止点
              
              if(absInt(StaticData.absForce-MotionPoint->absForce) < 300){
                  time++;
                  if(time == 3){
                      TIM1ON
                          time = 0;
                      printf("min2AbsForce:%u \n\r\n",min2AbsForce.absForce);
                      float a = StaticData.absForce-minAbsForce.absForce;
                      float b = StaticData.absForce-min2AbsForce.absForce; 
                      float c = a/b;
                      //printf("a:%u %u ",StaticData.absForce-minAbsForce.absForce,StaticData.absForce-min2AbsForce.absForce);
                      //printf("bi:%f",c);
                      /*switch(i){
                    case 0:printf("向下运动 by Case3 \n\r\n");break;
                    case 1:printf("向下来回运动 by Case3 \n\r\n");break;
                    case 2:printf("向下来回运动 by Case3 \n\r\n");break;
                      default:printf("向下运动 by Case3 \n\r\n");break;
                  }*/
                      void LEDCtrl(unsigned char temp);
                      if(timeOvarFlag)
                          printf("运动检测超时 by Case3 \n\r\n");
                      else
                      {
                          if(c>0.8&&c<3.0){
                              printf("向下来回运动 by Case3 \n\r\n");
                              LEDCtrl(4);
                              
                          }else
                              printf("向下运动 by Case3 \n\r\n");
                      }
                      switchState = 5;
                      break;
                  }     
              }
              else{
                  time = 0;
              }
              
              nextPoint(MotionPoint)
          }
          break;
          
          case 4:
          A:
            while(MotionPoint != HeadPoint){
                if(absInt(4096-MotionPoint->absForce) < 300){
                    time++;
                    if(time == 3){
                        time = 0;
                        //printf("向上运动");
                        TIM1ON
                            switchState = 5;
                        break;
                    }     
                }
                else{
                    time = 0;
                }
                
                nextPoint(MotionPoint);
            }
            break;
          case 5:
            if(timeOvarFlag == 1){
                EndPoint = MotionPoint;
                MotionPoint = NULL;
                switchState = 0;
                printf("重新检测 \n\r\n");
            }
            break;
        }
    }
}  

