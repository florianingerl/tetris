#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

#include <cstdlib> 
#include <time.h>

#include <cstdlib>
#include <ctime>
#include <functional>

#include <iostream>
#include <vector>
#include <list>

#include <thread>
#include <mutex>
#include <condition_variable>

#include "timer.h"

#define unit 20
using namespace cv;

void clearImage(Mat& img);

class tile  {

    public:

    virtual void draw(cv::Mat& m) = 0;

    virtual void moveDown() {}

    virtual void moveRight () {}; 

    virtual void moveLeft () {}; 

    virtual void settleDownOnBottom(std::vector<int> &heights) {};
    
    virtual bool isOnTheBottom(int height, std::vector<int> &heights) {
      return false;
    };

};

class hline : public tile {

    private:
        cv::Point position; //linke position
        int width;
        cv::Scalar colour; 


    public:
     hline(const cv::Point& _position, int _width, cv::Scalar _colour = cv::Scalar(0,255,0)):position(_position), width(_width), colour(_colour){
        //this->position = _position;
     }    

    void draw(Mat& img) override {
        int thickness = 2;
        int lineType = LINE_8;
        
 
        // this = Zeiger auf das Objekt der Klasse hline, dessen draw methode gerade ausgeführt wird.
        // *this wäre das Objekt selber
        // (*this).draw ruft dann die draw Methode dieses Objekts auf, this->draw ist Kurznotation für (*this).draw

        line( img,
              Point(position.x * unit, position.y * unit),
              Point( (position.x + this->width) * unit, position.y * unit),
              colour,
              thickness,
              lineType );
    }

    void moveDown() override {
        position.y = position.y + 1;
    }
    void moveRight() override {
        position.x = position.x + 1; 
    }
};

class vline : public tile{

    private: 
      cv::Point position;
      int height; 
      cv::Scalar colour;
      
    public: 
      vline(const cv::Point& _position, int _height, const cv::Scalar& _colour = Scalar(0,255,0)):position(_position),height(_height), colour(_colour){}
       
       void draw(cv::Mat& img) {
int thickness = unit;
  int lineType = LINE_8;
 
  line( img,
    Point(position.x * unit, position.y * unit),
    Point(position.x * unit, (position.y + height) * unit),
    colour,
    thickness,
    lineType );
       }
    

};

class square : public tile {

  private:
    cv::Point position;
    int width;

    public:
        square(cv::Point pos, int _width = 20): position(pos), width(_width){

        } 

        void moveDown( )  override  {
             position.y = position.y+1; 

        }

        void draw(cv::Mat& img) override {
            for(int i=0; i < width * unit; ++i ){
                for(int j=0; j < width * unit; ++j){
                    img.at<uchar>(position.y * unit + i, position.x * unit + j) = 200;
                }
            }
        
        } 
};

class Rectangle : public tile {
         
      private:
         int width;
         int height; 
         cv::Point pos;
         int colour; //Farbe ist eine Zahl zwischen 0 und 255

      public:

        Rectangle(cv::Point _pos, int _width, int _height, int _colour = 200): pos(_pos), width(_width), height(_height), colour(_colour){}
       
        void draw (cv::Mat& img) override{
           for(int i=0; i< width * unit ; i++){
               for(int j=0; j< height * unit; j++){
                 img.at<uchar>(pos.y * unit + j, pos.x * unit + i) = colour; //Farbe ist eine Zahl zwischen 0 und 255
               }
        
           }
        }
         void moveDown () override{
           pos.y = pos.y + 1; 
         }

         void moveRight() override {
        pos.x = pos.x +1; 
    }
        void moveLeft () override {
          pos.x = pos.x -1; 
        }

        bool isOnTheBottom (int height, std::vector<int>& heights) override {
          
          if (this-> pos.y >= height - this->height){
                return true; 
          } 
          else {
            std::cout<< "we are not on the bottom " << std::endl; 
            return false;
          }
          
        }

};

class opencvplatform {
  private:
     static std::function<void(void)> f;
     static std::condition_variable cv;
     static std::mutex mtx;

     static bool running;

  public:
    static void runLoop() {
       while(true){
         std::unique_lock<std::mutex> lock(mtx);
         cv.wait(lock, [&]{ return (!running) || f;  } ); //std::function is converted to a bool. It's true when the function pointer is not 0
         if(!running){
           lock.unlock();
           cv.notify_one();
           break;
         }
         f();
         f = std::function<void(void)>();
         lock.unlock();
         cv.notify_one();
       }
    }  

    static void invokeNow(std::function<void(void)> g ) {
        std::unique_lock<std::mutex> lock(mtx);
        cv.wait(lock, [&]{ return !f; } );
        f = g;
        lock.unlock();
        cv.notify_one();

        lock.lock();
        cv.wait(lock, [&]{ return !f; } );
        lock.unlock();

    } 

    static void exit(){
      	std::unique_lock<std::mutex> lock(mtx);
        cv.wait(lock, [&]{ return !f; });
        running = false;
        lock.unlock();
        cv.notify_one();
    }
};

std::mutex opencvplatform::mtx = std::mutex();
std::condition_variable opencvplatform::cv = std::condition_variable();
std::function<void(void)> opencvplatform::f = std::function<void(void)>();
bool opencvplatform::running = true;

class game {
    private:
        tile *currTile;
        cv::Mat img;

        int i;
        Timer *timer;

        std::list<tile *> tiles;
        std::vector<int> heights;

        int height;
        int width;
        int score = 0;

        std::condition_variable& cv;
        std::mutex& mtx;

    public:
        int key;

        game(int w, int h, std::condition_variable& _cv, std::mutex& _mtx): cv(_cv),mtx(_mtx),  height(h), width(w), heights(std::vector<int>(w,0) ), currTile(0) , i(0){
          img = Mat::zeros(h * unit, w * unit,  CV_8UC3);
          imshow( "Tetris", img );
          srand(time (0) );
          moveWindow( "Tetris", 200, 200 );
          waitKey(1);
          // use current time as seed for random generator
           
        } 

        void start(){
           
            timer = new Timer([&](){
		              this->next();
	          }, 1000);

            std::cout << "The timer will be started!" << std::endl;
	          timer->start(); //This method blocks, until t1 is finished!
          
        }


        void newTile() {
           int widthRect = 1;
           int random_value = std::rand()%(width-widthRect); //Random_value ist jetzt Zufallszahl zwischen u und 380
           currTile = new ::Rectangle( cv::Point(random_value,0),widthRect, 3, std::rand()%256 );
        }

        void next(){
            i = i+1;

            if( currTile == 0){
              newTile();
            }
            std::cout << "Trying to poll the key!" << std::endl;
            opencvplatform::invokeNow([&]{ this->key = cv::pollKey(); });
            std::cout << this->key << std::endl;
            /*
            key = -2;
            cv.notify_one();
            std::unique_lock<std::mutex> lock(mtx);
            cv.wait(lock, [&]() { return this->key != -2; });
            lock.unlock();
            std::cout << key << std::endl;
            */

            if(key == 27 ){ //Escape, then the game should end
               timer->stop();
               opencvplatform::exit();
               return;
            }

            if( currTile!= 0){
              
              if(key == 39){
                  currTile->moveRight();
              }
              else if(key == 37){
                  currTile->moveLeft();
              }

              else if (key == 40 && !currTile->isOnTheBottom(height, heights) ){
                currTile-> moveDown(); 
              }


              if(i % 3 == 0 && !currTile->isOnTheBottom(height, heights) ){
                  currTile->moveDown();
              }

              if(currTile->isOnTheBottom(height, heights) ){
                tiles.push_back(currTile);
                currTile->settleDownOnBottom(heights);
                currTile = 0;
              }

            }

            draw(); //Todo: Draw all the tiles , not only the current one

            /*
            std::cout << "Trying to call imshow" << std::endl;
            key = -3;
            cv.notify_one();
            lock.lock();
            cv.wait(lock, [&]{ return this->key == -4; });
            lock.unlock();
            std::cout << "Imshow was successful!" << std::endl; */
            opencvplatform::invokeNow( [&]{ cv::imshow("Tetris", this->img ); } );
 
        }

        void draw(){
            clearImage( img );
            if(currTile != 0) {
                currTile->draw(img);
            }

            /*for(auto it = tiles.begin(); it!=tiles.end(); ++it){
              (*it)->draw(img);
            }*/
            
            for (auto tile : tiles){
              tile->draw(img); 
            }

            //Todo: Draw all the tiles
        }    
};




int main( void ){
  
  std::mutex mtx;
  std::condition_variable cv;
//Todo: Game muss Breite und Höhe wissen
  ::game g(20, 40, cv, mtx);

  g.start();

  /*
  while(true){
    std::unique_lock<std::mutex> ulock(mtx);    
    cv.wait(ulock, [&]{ return g.key == -2; } );  //if wait condition is true ie start==false, go in
    g.key = cv::pollKey();

    ulock.unlock(); 
    if(g.key == 27){ //Escape
      break;
    }                                        //5c. unlock unique_lock.
    cv.notify_one(); 

    ulock.lock();
    cv.wait(ulock, [&]{ return g.key == -3; } );
    g.drawImage();
    g.key = -4;
    ulock.unlock();
    cv.notify_one();

  }

  cv.notify_one();*/
  opencvplatform::runLoop();

  return(0);
}


void clearImage( cv::Mat& img )
{
  rectangle( img,
      Point(0,0),
      Point(img.cols,img.rows),
      Scalar( 0, 0, 0 ),
      FILLED,
      LINE_8 );
}
