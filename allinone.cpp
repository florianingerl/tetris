#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>

#include <cstdlib>
#include <ctime>

#include <iostream>
#include <vector>
#include <list>

#include "timer.h"

#define unit 20
using namespace cv;

void clearImage(Mat img);

class tile  {

    public:

    virtual void draw(cv::Mat& m) = 0;

    virtual void moveDown() {}

    virtual void moveRight () {}; 

    virtual void moveLeft () {}; 

    virtual void settleDownOnBottom(std::vector<int> &heights) {};
    
    virtual void isOnTheBottom(std::vector<int> &heights) {};

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

      public:

        Rectangle(cv::Point _pos, int _width, int _height ): pos(_pos), width(_width), height(_height){}
       
        void draw (cv::Mat& img) override{
           for(int i=0; i< width * unit ; i++){
               for(int j=0; j< height * unit; j++){
                 img.at<uchar>(pos.y * unit + j, pos.x * unit + i) = 200;
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

};

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

        char[] tetris_window = "Tetris";

    public:

        game(int w, int h): height(h), width(w), heights(std::vector<int>(w,0) ), img( Mat::zeros(width * unit, height * unit) ), currTile(0) , i(0){
          imshow( tetris_window, img );
          moveWindow( tetris_window, 200, 200 );
          waitKey(1);
          std::srand(std::time(nullptr)); // use current time as seed for random generator
           
        } 

        void start(){
            timer = new Timer([&](){
		              this->next();
	          }, 1000);

	          t1.start(); //This method blocks, until t1 is finished!
          
        }


        void newTile() {
           int widthRect = 1;
           int random_value = std::rand()%(width-widthRect); //Random_value ist jetzt Zufallszahl zwischen u und 380
           currTile = new ::Rectangle( cv::Point(random_value,0), widthRect, 3 );
        }

        void next(){
            i = i+1;
            int key = pollKey();
            std::cout << key << std::endl;

            if(key == 4 ){ //Escape, then the game should end
               timer->stop();
               return;
            }

            if( currTile!= 0){
              
              if(key == 39){
                  currTile->moveRight();
              }
              else if(key == 37){
                  currTile->moveLeft();
              }

              if(i % 3 == 0){
                  currTile->moveDown();
              }

              if(currTile->isOnTheBottom(heights) ){
                tiles->push_back(currTile);
                currTile->settleDownOnBottom(heights);
                currTile = 0;
              }

            }

            draw(); //Todo: Draw all the tiles , not only the current one

            imshow( window, img );
            waitKey(1);

            
        }

        void draw(){
            clearImage( img );
            if(currTile != 0) {
                currTile->draw(img);
            }

            //Todo: Draw all the tiles
        }    
};


int main( void ){
  

//Todo: Game muss Breite und Höhe wissen
  ::game g(20, 40);

  g.start();

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
