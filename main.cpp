#include "SDL2/SDL_audio.h"
#include "SDL2/SDL_hints.h"
#include "SDL2/SDL.h"
#include "SDL2/SDL_blendmode.h"
#include "SDL2/SDL_events.h"
#include "SDL2/SDL_mouse.h"
#include "SDL2/SDL_pixels.h"
#include "SDL2/SDL_render.h"
#include "SDL2/SDL_rwops.h"
#include "SDL2/SDL_surface.h"
#include "SDL2/SDL_timer.h"
#include "SDL2/SDL_video.h"
#include "SDL2/SDL_ttf.h"
#include "SDL2/SDL_image.h"
#include "SDL2/SDL_mixer.h"
#include "SDL2/SDL_net.h"
#include <assert.h>
#include <cstdint>
#include <iostream>
#include <algorithm>
#include <vector>
#include <deque>
#include <string>


#define wi 1280
#define he 760
#define tw 20
#define th 20

static SDL_Window*   window            = nullptr;
static SDL_Renderer* renderer          = nullptr;
TTF_Font*            font              = nullptr;

class ImageItem
{
public:
  SDL_Surface* Surface                 = nullptr;
  SDL_Texture* TextureImg              = nullptr;
  SDL_Rect     RectImg;
  ImageItem()
  {
    
  }
  ImageItem(const std::string& path,int x,int y)
  {
    Surface=IMG_Load(path.c_str());
    TextureImg = SDL_CreateTextureFromSurface(renderer,Surface);
    RectImg.w=Surface->w;
    RectImg.h=Surface->h;
    RectImg.x=x;
    RectImg.y=y;
    SDL_FreeSurface(Surface);
  }
  void ImageItemInit(const std::string& path,int x,int y)
  {
    Surface=IMG_Load(path.c_str());
    TextureImg = SDL_CreateTextureFromSurface(renderer,Surface);
    RectImg.w=Surface->w;
    RectImg.h=Surface->h;
    RectImg.x=x;
    RectImg.y=y;
    SDL_FreeSurface(Surface);
  }

  void ImageGenerate(int x,int y,SDL_Color col,int w,int h)
  {
    Surface = SDL_CreateRGBSurface(0,w,h,32,0,0,0,0);
    SDL_FillRect(Surface, NULL, SDL_MapRGBA(Surface->format, col.r, col.g, col.b,255));
    TextureImg = SDL_CreateTextureFromSurface(renderer,Surface);
    RectImg.x=x;
    RectImg.y=y;
    RectImg.w = Surface->w;
    RectImg.h = Surface->h;
    SDL_FreeSurface(Surface);
  }
  void ImageItemFromArray(int x,int y)
  {
    TextureImg = SDL_CreateTextureFromSurface(renderer,Surface);
    RectImg.w=Surface->w;
    RectImg.h=Surface->h;
    RectImg.x=x;
    RectImg.y=y;
  }
  void setCoords(int x,int y)
  {
    RectImg.x=x;
    RectImg.y=y;
  }
  void draw()
  {
    SDL_RenderCopy(renderer,TextureImg,NULL,&RectImg);
  }
  void drawWithAlppha(int alpha)
  {
    SDL_SetTextureBlendMode(TextureImg,SDL_BLENDMODE_ADD);
    SDL_SetTextureAlphaMod( TextureImg, alpha );
    SDL_RenderCopy(renderer,TextureImg,NULL,&RectImg);
  }
  ~ImageItem()
  {
    SDL_DestroyTexture(TextureImg);
  }
};

class TextItem
{
public:
  SDL_Surface* Surface                  = nullptr;
  SDL_Texture* TextureText              = nullptr;
  SDL_Rect     RectText;
  SDL_Color    color;
public:
  TextItem()
  {
  }
  TextItem(const std::string& str,SDL_Color color,int x,int y)
  {
    Surface=TTF_RenderUTF8_Blended(font,str.c_str(),color);
    TextureText=SDL_CreateTextureFromSurface(renderer,Surface);
    RectText.x=x;
    RectText.y=y;
    RectText.w=Surface->w;
    RectText.h=Surface->h;
    SDL_FreeSurface(Surface);
  }
  void TextItemInit(const std::string& str,SDL_Color col,int x,int y)
  {
    Surface=TTF_RenderUTF8_Blended(font,str.c_str(),col);
    TextureText=SDL_CreateTextureFromSurface(renderer,Surface);
    RectText.x=x;
    RectText.y=y;
    RectText.w=Surface->w;
    RectText.h=Surface->h;
    SDL_FreeSurface(Surface);
  }
  void setCoords(int x,int y)
  {
    RectText.x=x;
    RectText.y=y;
  }
  void setColor(Uint8 r,Uint8 g,Uint8 b)//update
  {
    SDL_SetTextureColorMod(TextureText,r,g,b);
  }
  void draw()
  {
    SDL_RenderCopy(renderer,TextureText,NULL,&RectText);
  }

  ~TextItem()
  {
    SDL_DestroyTexture(TextureText);
    
  }
};

class MySystem
{
public:
  MySystem()
  {

#ifdef SDL_VIDEO_DRIVER_X11

    setenv("SDL_VIDEODRIVER","x11");
#endif
    SDL_Init(SDL_INIT_EVERYTHING | SDL_INIT_TIMER|SDL_INIT_AUDIO);
    int flags = MIX_INIT_MP3;
    Mix_Init(flags);
    SDL_CreateWindowAndRenderer(wi,he,SDL_RENDERER_ACCELERATED | SDL_WINDOW_ALLOW_HIGHDPI,&window,&renderer);
    
    SDL_SetWindowTitle(window,"Snake");
    TTF_Init();


    font = TTF_OpenFont("res/Roboto-Light.ttf",44);

    
    IMG_Init(IMG_INIT_PNG);
  }
  void clear()
  {
    SDL_SetRenderDrawColor(renderer,0,0,0,SDL_ALPHA_OPAQUE);
    SDL_RenderClear(renderer);
    SDL_SetRenderDrawColor(renderer,255,255,255,SDL_ALPHA_TRANSPARENT);
  }
  void draw()
  {
    SDL_RenderPresent(renderer);
  }
  ~MySystem()
  {
    TTF_Quit();
    IMG_Quit();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    Mix_Quit();
    SDL_Quit();
  }
};
class Screen
{
public:
  SDL_Surface* surface = nullptr;
  SDL_Texture* texture = nullptr;
  SDL_Rect     rectangle;
  Screen()
  {
    surface = SDL_CreateRGBSurface(0,wi,he,32,0,0,0,0);
    SDL_FillRect(surface, NULL, SDL_MapRGBA(surface->format, 25, 20, 20, 0xff));
    texture = SDL_CreateTextureFromSurface(renderer,surface);
    rectangle.x=rectangle.y=0;
    rectangle.w = surface->w;
    rectangle.h = surface->h;
  }
  void draw()
  {
    SDL_RenderCopy(renderer,texture,NULL,&rectangle);
  }
  ~Screen()
  {
    SDL_DestroyTexture(texture);
    SDL_FreeSurface(surface);
  }
};

enum dir
  {
    LEFT=0,
    RIGHT,
    UP,
    DOWN
  };
enum tiles
  {
    wall=0,
    place,
  };
class TileMapScreen
{
public:
  SDL_Surface* surface1 = nullptr;//light
  SDL_Texture* texture1 = nullptr;//light
  SDL_Rect     rectangle1;//light
  SDL_Surface* surface2 = nullptr;//shadow
  SDL_Texture* texture2 = nullptr;//shadow
  SDL_Rect     rectangle2;//shadow
  std::vector<std::vector<SDL_Rect>> tilemap;
  TileMapScreen()
  {
    surface1 = SDL_CreateRGBSurface(0,tw,th,32,0,0,0,0);
    SDL_FillRect(surface1, NULL, SDL_MapRGBA(surface1->format, 100, 100, 100, 255));
    texture1 = SDL_CreateTextureFromSurface(renderer,surface1);
    rectangle1.x=rectangle1.y=0;
    rectangle1.w = surface1->w;
    rectangle1.h = surface1->h;
    surface2 = SDL_CreateRGBSurface(0,tw,th,32,0,0,0,0);
    SDL_FillRect(surface2, NULL, SDL_MapRGBA(surface2->format, 200, 200, 200, 0));
    texture2 = SDL_CreateTextureFromSurface(renderer,surface2);
    SDL_SetTextureBlendMode(texture2,SDL_BLENDMODE_ADD);
    rectangle2.x=rectangle2.y=0;
    rectangle2.w = surface2->w;
    rectangle2.h = surface2->h;
    SDL_FreeSurface(surface1);
    SDL_FreeSurface(surface2);
    tilemap.resize(he/th);
    for(int i = 0;i<(int)tilemap.size();i++)
      {
 	tilemap[i].resize(wi/tw);
      }
    int count     =0;
    int countLine =0;
    int countWidth=0;
    int countW    =0;
    for(auto& a: tilemap)
      {
 	if(countLine==2)countLine=0;
 	countLine++;
 	for(auto& b:a )
 	  {
 	    if(countW==2)countW=0;
 	    countW++;
 	    if(countLine==1)
 	      {
 		if(countW==1)
 		  {
   
 		    b=rectangle1;
 		    b.x=countWidth*tw;
 		    b.y=count*th;
 		  }
 		if(countW==2)
 		  {
		    
 		    b=rectangle2;
 		    b.x=countWidth*tw;
 		    b.y=count*th;
 		  }
		
 	      }
 	    if(countLine==2)
 	      {
 		if(countW==1)
 		  {
	    
 		    b=rectangle1;
 		    b.x=countWidth*tw;
 		    b.y=count*th;
 		  }
 		if(countW==2)
 		  {
		    
 		    b=rectangle2;
 		    b.x=countWidth*tw;
 		    b.y=count*th;
 		  }
 	      }
 	    countWidth++;
 	  }
 	count++;
 	countWidth=0;
      }
  }
  void draw()
  {
    int count     =0;
    int countLine =0;
    int countWidth=0;
    int countW    =0;
    for(auto& a: tilemap)
      {
	
 	if(countLine==2)countLine=0;
 	countLine++;
 	for(auto& b:a )
 	  {
 	    if(countW==2)countW=0;
 	    countW++;
 	    if(countLine==1)
 	      {
 		if(countW==1)
 		  {
 		    SDL_SetTextureBlendMode(texture1,SDL_BLENDMODE_ADD);
 		    SDL_SetTextureAlphaMod( texture1, 20 );
 		    SDL_RenderCopy(renderer,texture1,NULL,&b);
 		  }
 		if(countW==2)
 		  {
 		    SDL_SetTextureBlendMode(texture2,SDL_BLENDMODE_ADD);
 		    SDL_SetTextureAlphaMod( texture2, 20 );
 		    SDL_RenderCopy(renderer,texture2,NULL,&b);
 		  }
		
 	      }
 	    if(countLine==2)
 	      {
 		if(countW==1)
 		  {
 		    SDL_SetTextureBlendMode(texture2,SDL_BLENDMODE_ADD);
 		    SDL_SetTextureAlphaMod( texture2, 20 );
 		    SDL_RenderCopy(renderer,texture2,NULL,&b);
 		  }
 		if(countW==2)
 		  {
 		    SDL_SetTextureBlendMode(texture1,SDL_BLENDMODE_ADD);
 		    SDL_SetTextureAlphaMod( texture1, 20 );
 		    SDL_RenderCopy(renderer,texture1,NULL,&b);
 		  }
 	      }
 	    countWidth++;
 	  }
 	count++;
 	countWidth=0;
      }
  
  }
  ~TileMapScreen()
  {
    SDL_DestroyTexture(texture1);
    
    SDL_DestroyTexture(texture2);
  }
};

class Head
{
public:
  SDL_Surface* surface1 = nullptr;
  SDL_Texture* texture1 = nullptr;
  SDL_Rect     rectangle1;
  int direction = 0;
  std::deque<SDL_Rect> rq;
  int size      = 1;

  Head()
  {

  
    surface1 = SDL_CreateRGBSurface(0,tw,th,32,0,0,0,0);
    SDL_FillRect(surface1, NULL, SDL_MapRGBA(surface1->format, 100, 100, 110, 0xff));
    texture1 = SDL_CreateTextureFromSurface(renderer,surface1);
  
    rectangle1.x=rectangle1.y=60;
    rectangle1.w = surface1->w;
    rectangle1.h = surface1->h;
    SDL_FreeSurface(surface1);
  }
  void calculate(bool move)
  {
    if(rectangle1.x%20==0&&rectangle1.y%20==0)
      {
	if(move==true)
	  switch(direction)
	    {
	    case DOWN:
	      SDL_Delay(100);
	      rectangle1.y+=th;break;
	    case UP:
	      SDL_Delay(100);
	      rectangle1.y-=tw;break;
	    case LEFT:
	      SDL_Delay(100);
	      rectangle1.x-=tw;break;
	    case RIGHT:
	      SDL_Delay(100);
	      rectangle1.x+=th;break;
	    }
      }
  }
  void draw(bool& move,bool& right,bool& left,bool& up,bool& down,bool& gameOver,bool& win)
  {
    std::for_each(rq.begin(),rq.end(),[&](auto& snake_seg)
    {
      if(rectangle1.x==snake_seg.x&& rectangle1.y==snake_seg.y)
 	{
	  //gameOver=true;
	  if(size>10)
	    {
	      if(!win)gameOver=true;
	      else win=true;
	    }
	  move=false;
 	}
    });
    rq.push_front(rectangle1);
    while((int)rq.size()>size)
      rq.pop_back();

    std::for_each(rq.begin(),rq.end(),[&](auto& snake_seg)
    {
      SDL_RenderCopy(renderer,texture1,NULL,&snake_seg);
    });
  }
  ~Head()
  {
    SDL_DestroyTexture(texture1);
  }
};

class Appless
{
public:
  SDL_Surface* surface1 = nullptr;//light
  SDL_Texture* texture1 = nullptr;//light
  SDL_Rect     rectangle1;//
  std::vector<SDL_Rect> apples;
  int count = 0;
  Appless()
  {


    surface1 = SDL_CreateRGBSurface(0,tw,th,32,0,0,0,0);
    SDL_FillRect(surface1, NULL, SDL_MapRGBA(surface1->format, 100, 100, 100, 255));
    texture1 = SDL_CreateTextureFromSurface(renderer,surface1);
    
    rectangle1.x=rectangle1.y=0;
    rectangle1.w = surface1->w;
    rectangle1.h = surface1->h;
    SDL_FreeSurface(surface1);
  }
  void generate(int n)
  {
    for(int i= 0;i<n;i++)
      {
 	apples.emplace_back(rand()%wi/tw*tw,rand()%he/th*th,tw,th);
      }
    count = apples.size();
  }
  void draw()
  {
    SDL_SetRenderDrawColor(renderer,155 ,0,50,255);
    std::for_each(apples.begin(),apples.end(),[&](auto& apples_seg)
    {
      SDL_RenderCopy(renderer,texture1,NULL,&apples_seg);
    });
  }
  ~Appless()
  {
    SDL_DestroyTexture(texture1);
  }
};

class InSys
{
public:
  bool run;
  bool down     =false;
  bool up       =false;
  bool left     =false;
  bool right    =false;
  bool move     =false;
  ///////////////////
  bool bPlay    =false;
  bool sPlay    =false;
  bool bQuit    =false;
  //////////////////
  bool gbPlay   =false;
  bool gsPlay   =false;
  bool gbQuit   =false;
  //////////////////
  bool wbPlay  =false;
  bool wsPlay  =false;
  bool wbQuit  =false;
  //////////////////
  bool gameover =false;
  bool win      =false;
  //////////////////
  bool mainmenu =false;
  //////////////////
  bool generateAfterGameOver =false;

  SDL_Point pt;
  InSys()
  {
  }
  bool calculate(Head& head,SDL_Rect Play,SDL_Rect Quit,SDL_Rect gPlay,SDL_Rect gQuit,SDL_Rect wPlay,SDL_Rect wQuit)
  {
    SDL_Event e;
    SDL_GetMouseState(&pt.x, &pt.y);
    while(SDL_PollEvent(&e))
      {
 	switch(e.type)
 	  {
	  case SDL_MOUSEBUTTONDOWN:
	    if(mainmenu)
	      {
		if (e.button.button == SDL_BUTTON_LEFT && SDL_PointInRect(&pt, &Quit))//+draw rect for selecting
		  {
		    run=true;
		  }
		if (e.button.button == SDL_BUTTON_LEFT && SDL_PointInRect(&pt, &Play))//+draw rect for selecting
		  {
		    bPlay=true;
		  }
	      }
	    else{}
	    if(gameover)
	      {
		if (e.button.button == SDL_BUTTON_LEFT && SDL_PointInRect(&pt, &gQuit))//+draw rect for selecting
		  {
		    run=true;
		  }
		if (e.button.button == SDL_BUTTON_LEFT && SDL_PointInRect(&pt, &gPlay))//+draw rect for selecting
		  {
		    gbPlay=true;
		    generateAfterGameOver=true;
		  }
	      }
	    else{}
	    if(win)
	      {
		if (e.button.button == SDL_BUTTON_LEFT && SDL_PointInRect(&pt, &wQuit))//+draw rect for selecting
		  {
		    run=true;
		  }
		if (e.button.button == SDL_BUTTON_LEFT && SDL_PointInRect(&pt, &wPlay))//+draw rect for selecting
		  {
		    wbPlay=true;
		    generateAfterGameOver=true;
		  }
	      }
	    else{}
	    break;
	  case SDL_MOUSEMOTION:
	    if (SDL_PointInRect(&pt, &Play))
	      {
		sPlay = true;
	      }
	    else if (SDL_PointInRect(&pt, &Quit))
	      {

		bQuit = true;
	      }
	    else
	      {
		sPlay=false;
		bQuit=false;
	      }
	    if (SDL_PointInRect(&pt, &gPlay))
	      {
		gsPlay = true;
	      }
	    else if (SDL_PointInRect(&pt, &gQuit))
	      {

		gbQuit = true;
	      }
	    else
	      {
		gsPlay=false;
		gbQuit=false;
	      }
	    if (SDL_PointInRect(&pt, &wPlay))
	      {
		wsPlay = true;
	      }
	    else if (SDL_PointInRect(&pt, &wQuit))
	      {

		wbQuit = true;
	      }
	    else
	      {
		wsPlay=false;
		wbQuit=false;
	      }  
	    break;
 	  case SDL_KEYDOWN:
	    if(!gameover&&!win)
	      {
		if(e.key.keysym.sym==SDLK_d)
		  {
		    move=true;
		    if(!left)
		      {
			right=true;
			up=false;
			down=false;
			head.direction = RIGHT;
			left=false;
		      }
		  }
		if(e.key.keysym.sym==SDLK_a)
		  {
		    move=true;
		    if(!right)
		      {
			left=true;
			up=false;
			down=false;
			head.direction = LEFT;
			right=false;
		      }
		  }
		if(e.key.keysym.sym==SDLK_w)
		  {
		    move=true;
		    if(!down)
		      {
			up=true;
			left=false;
			right=false;
			head.direction = UP;
			down=false;

		      }
		  }
		if(e.key.keysym.sym==SDLK_s)
		  {
		    move=true;
		    if(!up)
		      {
			down=true;
			left=false;
			right=false;
			head.direction = DOWN;
			up=false;
		      }
		  }
	      }
 	    break;
 	  case SDL_QUIT:
 	    run = true;
 	    break;
 	  }
      }
    return run;
  }
  ~InSys()
  {

  }
};

class Timerr
{
public:
  Uint32 start;
  Uint32 end;
  Uint32 tfd=1000/60;
  Timerr()
  {}
  void starts()
  {
    start = SDL_GetTicks();
  }
  void ends()
  {
    end = SDL_GetTicks()-start;
  }
  void delays()
  {
    if((tfd)> end)
      SDL_Delay((tfd)-(SDL_GetTicks()-start));
    
  }
  ~Timerr()
  {
    
  }
};

class Arbitr
{
public:
  Arbitr()
  {
  }
  void equation(Head& head,Appless& appless,InSys& sys,bool& game_over,bool& win)
  {
    std::for_each(appless.apples.begin(),appless.apples.end(),[&](auto& apples_segments)
    {
      if(head.rectangle1.x==apples_segments.x&&head.rectangle1.y==apples_segments.y)
 	{
 	  head.size+=2;
 	  apples_segments.x=-tw;
 	  apples_segments.y=-th;
	  appless.count--;
 	}
      if(head.rectangle1.x+tw>wi || head.rectangle1.y+th>he)
	{
	  sys.left     =true;
	  sys.right    =true;
	  sys.down     =true;
	  sys.up       =true;
	  sys.move     =false;
	  game_over    =true;
	  head.rectangle1.x-=20;
	  head.rectangle1.y-=20;
	}
      if( head.rectangle1.x<0 || head.rectangle1.y<0)//left
	{
	  sys.left     =true;
	  sys.right    =true;
	  sys.down     =true;
	  sys.up       =true;
	  sys.move     =false;
	  game_over    =true;
	  head.rectangle1.x+=20;
	  head.rectangle1.y+=20;
	}
      if(appless.count==0)
	{
	  sys.left     =true;
	  sys.right    =true;
	  sys.down     =true;
	  sys.up       =true;
	  sys.move     =false;
	  //game_over    =false;
	  win          = true;
	}
    });
  }
  ~Arbitr()
  {
  }
};

class MainMenu
{
public:
  //////////////////////////////////////////////MenuData
  std::string Play;
  std::string Game_Over;
  std::string Quit;
  //////////////////////////////////////////////LevelData
  std::string TimeFromStartOnLevel;
  std::string ScoreOnLevel;//prgressOnLevel
  std::string PauseOnLevel;//stopTime;

  TextItem  playText;

  TextItem  quitText;

  ImageItem backGameOver;
  TextItem  gameOver;
  
  ImageItem labelGame;
  
public:
  MainMenu()
  {
    Play      +="Play";
    Game_Over +="Game Over";
    Quit      +="Quit";

    playText.TextItemInit(Play,SDL_Color{255,255,255,255},wi/2,he/2);
    playText.setCoords((wi/2)-(playText.Surface->w/2),((he/2)-(playText.Surface->h/2)+20));

    quitText.TextItemInit(Quit,SDL_Color{255,255,255,255},0,0);
    quitText.setCoords((wi/2)-(quitText.Surface->w/2),((he/2)+(quitText.Surface->h/2)+40));

    labelGame.ImageItemInit("res/SnakeLabel.png",0,0);
    labelGame.setCoords((wi/2)-(labelGame.Surface->w/2),(he/50)-(labelGame.Surface->h/50));

    //GameOverText
    gameOver.TextItemInit(Game_Over,SDL_Color{177,177,177},wi/2,he/2);
    gameOver.setCoords((wi/2)-(gameOver.Surface->w/2),((he/2)-(gameOver.Surface->h/2)));

    backGameOver.ImageGenerate(0,0,SDL_Color{6,15,15,0},wi,he);
   
  }
  void draw()
  {
    labelGame.draw();
    
    playText.draw();
    
    quitText.draw();

  }

  ~MainMenu()
  {
  }
};

class WinOverlay
{
public:
  std::string wPlay;
  std::string wYouWin;
  std::string wQuit;

  TextItem wplayText;

  TextItem wquitText;

  ImageItem backyouWin;
  TextItem wyouWin;

  
  WinOverlay()
  {
    wPlay      +="Play";
    wYouWin    +="Congratulations! You are Win";
    wQuit      +="Quit";

    wplayText.TextItemInit(wPlay,SDL_Color{255,255,255,255},wi/2,he/2);
    wplayText.setCoords((wi/2)-(wplayText.Surface->w/2),((he/2)-(wplayText.Surface->h/2)+60));

    wquitText.TextItemInit(wQuit,SDL_Color{255,255,255,255},0,0);
    wquitText.setCoords((wi/2)-(wquitText.Surface->w/2),((he/2)+(wquitText.Surface->h/2)+80));

    //GameOverText
    wyouWin.TextItemInit(wYouWin,SDL_Color{177,177,177},wi/2,he/2);
    wyouWin.setCoords((wi/2)-(wyouWin.Surface->w/2),((he/2)-(wyouWin.Surface->h/2)));

    backyouWin.ImageGenerate(0,0,SDL_Color{6,15,15,0},wi,he);
  }
  void draw()
  {
    backyouWin.drawWithAlppha(255);
    wyouWin.draw();

    wplayText.draw();
    
    wquitText.draw();
  }
  ~WinOverlay()
  {
    
  }
};

class GameOverOverlay
{
public:
  //////////////////////////////////////////////MenuData
  std::string gPlay;
  std::string gGame_Over;
  std::string gQuit;
  //////////////////////////////////////////////LevelData
  std::string TimeFromStartOnLevel;
  std::string ScoreOnLevel;//prgressOnLevel
  std::string PauseOnLevel;//stopTime;

  TextItem  gplayText;

  TextItem  gquitText;

  ImageItem backGameOver;
  TextItem  ggameOver;
  
public:
  GameOverOverlay()
  {
    gPlay      +="Play";
    gGame_Over +="Game Over";
    gQuit      +="Quit";

    gplayText.TextItemInit(gPlay,SDL_Color{255,255,255,255},wi/2,he/2);
    gplayText.setCoords((wi/2)-(gplayText.Surface->w/2),((he/2)-(gplayText.Surface->h/2)+60));

    gquitText.TextItemInit(gQuit,SDL_Color{255,255,255,255},0,0);
    gquitText.setCoords((wi/2)-(gquitText.Surface->w/2),((he/2)+(gquitText.Surface->h/2)+80));

    //GameOverText
    ggameOver.TextItemInit(gGame_Over,SDL_Color{177,177,177},wi/2,he/2);
    ggameOver.setCoords((wi/2)-(ggameOver.Surface->w/2),((he/2)-(ggameOver.Surface->h/2)));

    backGameOver.ImageGenerate(0,0,SDL_Color{6,15,15,0},wi,he);
    
  }
  void drawGameOver()
  {
    backGameOver.drawWithAlppha(255);
    ggameOver.draw();

    gplayText.draw();
    
    gquitText.draw();
    
  }
  ~GameOverOverlay()
  {
  }
};

class MusicItem
{
public:
  int audio_volume = MIX_MAX_VOLUME;
  Mix_Music *music = NULL;
  int looping = 0;
  int interactive = 0;
  int rwops = 0;
  //int i;
  const char *typ;
  const char *tag_title = NULL;
  const char *tag_artist = NULL;
  const char *tag_album = NULL;
  const char *tag_copyright = NULL;
  double loop_start, loop_end, loop_length, current_position;
  SDL_AudioSpec spec;
  std::string strFile;
  MusicItem()
  {
    spec.freq = MIX_DEFAULT_FREQUENCY;
    spec.format = MIX_DEFAULT_FORMAT;
    spec.channels = MIX_DEFAULT_CHANNELS;
  }
  void playEffect()
  {

  }
  void playBackSoud(const std::string& str)
  {
    strFile=str;

    Mix_OpenAudio(spec.freq, spec.format, spec.channels, 4096);
    music = Mix_LoadMUS_RW(SDL_RWFromFile((const char*)strFile.c_str(), "rb"), SDL_TRUE);
    Mix_VolumeMusic(audio_volume);
    Mix_SetMusicCMD(SDL_getenv("MUSIC_CMD"));
    typ = "MP3";
    // tag_title = Mix_GetMusicTitleTag(music);
    // loop_start = Mix_GetMusicLoopStartTime(music);
    // loop_end = Mix_GetMusicLoopEndTime(music);
    // loop_length = Mix_GetMusicLoopLengthTime(music);
    Mix_FadeInMusic(music,looping,2000);
  }

  void setVolume(int vol)
  {
    audio_volume=vol;
    Mix_VolumeMusic(audio_volume);
  }
  void setLoop(int l)
  {
    looping=l;
  }
  ~MusicItem()
  {
    Mix_FreeMusic(music);
    music = NULL;
    if(Mix_PlayingMusic()) {
      Mix_FadeOutMusic(1500);
      SDL_Delay(1500);
    }
    if (music) {
      Mix_FreeMusic(music);
      music = NULL;
    }
    Mix_CloseAudio();

  }
};

int main(int argc, char** argv)
{


  static MusicItem musi;
  
  musi.setLoop(20);
  musi.setVolume(20);
  musi.playBackSoud("res/TributeSnake.mp3");

  static MySystem sdlsystem;

  static Screen screen;
 
  Head head;
  
  static TileMapScreen tilemapMenu;
  static TileMapScreen tilemapGame;
  
  Appless appless;

  srand(time(NULL));
  appless.generate(rand()%20);
  
  static bool run = false;

  InSys input;
  input.run=run;

  static Timerr times;

  static Arbitr arbitr;

  static MainMenu mainmenu;


  static GameOverOverlay gameOverOverlay;

  static WinOverlay winOverlay;
  
  bool Game_Over=false;

  while(!run)
    {
      times.starts();

      run = input.calculate(head,mainmenu.playText.RectText,mainmenu.quitText.RectText,gameOverOverlay.gplayText.RectText,gameOverOverlay.gquitText.RectText,winOverlay.wplayText.RectText,winOverlay.wquitText.RectText);
      sdlsystem.clear();

      if(input.bPlay==false)
	{
	  input.mainmenu=true;
	  if(input.sPlay==true)
	    {
	      mainmenu.playText.setColor(150,100,100);
	    }
	  if(input.sPlay==false)
	    {
	      mainmenu.playText.setColor(255,255,255);
	    }
	  if(input.bQuit==true)
	    {
	      mainmenu.quitText.setColor(150,100,100);
	    }
	  if(input.bQuit==false)
	    {
	      mainmenu.quitText.setColor(255,255,255);
	    }

	  tilemapMenu.draw();
	  
	  mainmenu.draw();
	}

      if(input.bPlay==true||input.gbPlay==true||input.wbPlay==true)
	{
	  input.mainmenu=false;
	  //input.gameover=false;
	  //input.win=false;
	  head.calculate(input.move);
	  
	  arbitr.equation(head,appless,input,input.gameover,input.win);

	  screen.draw();
	  
	  tilemapGame.draw();

	  appless.draw();
      
	  head.draw(input.move,input.right,input.left,input.up,input.down,input.gameover,input.win);

	}

      if(input.gameover==true)
       	{
	  //input.gameover=true;
	  input.win=false;
       	  gameOverOverlay.drawGameOver();
       	  if(input.gsPlay==true)
       	    {
       	      gameOverOverlay.gplayText.setColor(150,100,100);
       	    }
       	  if(input.gsPlay==false)
       	    {
       	      gameOverOverlay.gplayText.setColor(255,255,255);
       	    }
       	  if(input.gbQuit==true)
       	    {
       	      gameOverOverlay.gquitText.setColor(150,100,100);
       	    }
       	  if(input.gbQuit==false)
       	    {
       	      gameOverOverlay.gquitText.setColor(255,255,255);
       	    }
       	}
        if(input.win==true)
       	{
	  input.win=true;
	  //input.gameover=false;
       	  winOverlay.draw();
       	  if(input.wsPlay==true)
       	    {
       	      winOverlay.wplayText.setColor(150,100,100);
       	    }
       	  if(input.wsPlay==false)
       	    {
       	      winOverlay.wplayText.setColor(255,255,255);
       	    }
       	  if(input.wbQuit==true)
       	    {
       	      winOverlay.wquitText.setColor(150,100,100);
       	    }
       	  if(input.wbQuit==false)
       	    {
       	      winOverlay.wquitText.setColor(255,255,255);
       	    }
       	}
      if(input.generateAfterGameOver==true)
       	{
	  srand(time(NULL));
       	  head.size=1;
       	  head.rectangle1.x=rand()%wi/tw*tw;
       	  head.rectangle1.y=rand()%he/th*th;
	  appless.generate(rand()%110);
       	  input.generateAfterGameOver=false;
	  input.gameover=false;
	  input.win=false;
	  input.left=false;
	  input.right=false;
	  input.up=false;
	  input.down=false;
	  input.move=false;
	  input.mainmenu=false;
       	}
      
      sdlsystem.draw();
      times.ends();
      times.delays();
    }
}
