
/* Game By: Erick Duarte
 * Utils Used: Dr. Freudenthal's shape-motion-demo, p2sw-demo, shape-Demo, and all libraries
 *provided 
 *
 */

/* Save the ship game consists of a red panel on the left side of 
the screen and a green one on the right side of the screen. Button s1 
moves BOTH of these panels up, s2 moves them down, s3 brings them closer together
and s4 widens the distance between these two panels. The ship is in color purple
and is a sliced rectangle, the purpose of the game is to keep the ship from touching
the left and right, wall (color white) by controlling the direction of the ship by touching the
 yellow ball with either panel. When either panel touch the yellow ball the direction of the ship 
changes to that of the opposite direction. The yellow ball is needed since the panels can't move 
the ship themselves as the ship is too heavy (and it is cheating). If either one of the panels, 
ball or ship touch either of these walls (white left and right) the game is over as the player 
loses.*/
#include <stdio.h>
#include <msp430.h>
#include <libTimer.h>
#include <lcdutils.h>
#include <lcddraw.h>
#include <p2switches.h>
#include <shape.h>
#include <abCircle.h>
#include "ship_buzzer.h"

#define GREEN_LED BIT6
#define REC_WIDTH 2
#define REC_LENGTH 10
static int addPoint;


int abSlicedRectCheck(const AbRect *rect, const Vec2 *centerPos, const Vec2 *pixel){
  Vec2 relPos;
  //vector from cener of pixel
  vec2Sub(&relPos, pixel, centerPos);
  //reject pixels in specified sclice
  if( relPos.axes[0] == 0 && relPos.axes[0]/2 == relPos.axes[0] &&
      relPos.axes[1]/2 != relPos.axes[1]){
    return 0;
  }else{
    return abRectCheck(rect,centerPos,pixel);
  }
}

AbRect slicedRectangle = {
 abRectGetBounds, abSlicedRectCheck, {10,5}
};

AbRect rectanglePanel = {
  abRectGetBounds, abRectCheck, {REC_WIDTH,REC_LENGTH}
}; 

AbRect rectangleLine = {
  abRectGetBounds, abSlicedRectCheck, {5,70}
}; 

AbRect square = {
   abRectGetBounds, abRectCheck, {3,3}
};

 AbRectOutline fieldOutline = {
   //playing field 
  abRectOutlineGetBounds, abRectOutlineCheck,   
  {screenWidth/2-5 , screenHeight/2-5 }
};

Layer layer5 = {
  //blue division rectangle
  (AbShape *)&rectangleLine,
  //bit below & right of center 
  {(screenWidth/2)+2, (screenHeight/2)+2},
  //last & next pos 
  {0,0}, {0,0},				    
  COLOR_WHITE,
  0
 };

Layer layer4 = {
  //blue square
  (AbShape *)&slicedRectangle,
  //bit below & right of center 
  {(screenWidth/2)-10, (screenHeight/2)-10},
  //last & next pos 
  {0,0}, {0,0},				    
  COLOR_CYAN,
  &layer5,
 };

Layer layer3 = {
  //Layer with yellow ball
  (AbShape *)&circle3,
  //bit below & right of center 
  {(screenWidth/2)+10, (screenHeight/2)+10},
  //last & next pos
  {0,0}, {0,0},				  
  COLOR_YELLOW,
  &layer4,  
};


 Layer fieldLayer = {
   //playing field as a layer 
   (AbShape *) &fieldOutline,
   {screenWidth/2, screenHeight/2}, 
   {0,0}, {0,0},                       
  COLOR_WHITE,
   &layer3,
  };

Layer layer1 = {
  //Layer with a red rectangle 
  (AbShape *)&rectanglePanel,
  //center
  {(screenWidth/2)-48, (screenHeight/2)+5},
  //last & next pos 
  {0,0}, {0,0},				    
  COLOR_RED,
  &fieldLayer,
};
 
Layer layer0 = {
  //Layer with green rectangle
  (AbShape *)&rectanglePanel,
  //bit below & right of center 
  {(screenWidth/2)+50, (screenHeight/2)+5},
   //last & next pos 
  {0,0}, {0,0},				   
  COLOR_GREEN,
  &layer1,
};

/** Moving Layer
 *  Linked list of layer references
 *  Velocity represents one iteration of change (direction & magnitude)
 */
typedef struct MovLayer_s {
  Layer *layer;
  Vec2 velocity;
  struct MovLayer_s *next;
} MovLayer;

MovLayer ml4 = { &layer4, {1,-2}, 0 };//speed and direction of prople ship
MovLayer ml3 = { &layer3, {-1,2}, &ml4 }; //add a 0 in &ml4 for blue square not to move
MovLayer ml1 = { &layer1, {0,1}, &ml3 }; 
MovLayer ml0 = { &layer0, {0,1}, &ml1 }; 

movLayerDraw(MovLayer *movLayers, Layer *layers){

  int row, col;
  MovLayer *movLayer;
  //disable interrupts (GIE off) 
  and_sr(~8);
  for (movLayer = movLayers; movLayer; movLayer = movLayer->next) { // for each moving layer 
    Layer *l = movLayer->layer;
    l->posLast = l->pos;
    l->pos = l->posNext;
  }
  or_sr(8);
  //disable interrupts (GIE on) 
  for (movLayer = movLayers; movLayer; movLayer = movLayer->next) { //for each moving layer 
    Region bounds;
    layerGetBounds(movLayer->layer, &bounds);
     lcd_setArea(bounds.topLeft.axes[0], bounds.topLeft.axes[1], 
		bounds.botRight.axes[0], bounds.botRight.axes[1]);
    for (row = bounds.topLeft.axes[1]; row <= bounds.botRight.axes[1]; row++) {
      for (col = bounds.topLeft.axes[0]; col <= bounds.botRight.axes[0]; col++) {
	Vec2 pixelPos = {col, row};
	u_int color = bgColor;
	Layer *probeLayer;
	for (probeLayer = layers; probeLayer; probeLayer = probeLayer->next) {
	  //probe all layers, in order 
	  if (abShapeCheck(probeLayer->abShape, &probeLayer->pos, &pixelPos)) {
      	    color = probeLayer->color;
      	    break; 
	  } //if probe check	  
	} //for checking all layers at col, row
	lcd_writeColor(color);
      } //for col
     } //for row
  } //for moving layer being updated
}

void checkForCollision(Layer *layer0, Layer *layer1, Layer *layer3, Layer *layer4,
		       MovLayer *ml, Region *fence){
  //draws layers and checks for collisions for drawing advances of layer
  int recRadius = (REC_WIDTH/2);
  Layer *greenPanel = layer0;
  Layer *redPanel = layer1;
  Layer *ball = layer3;
  Layer *ship = layer4;
  Vec2 newPos;
  u_char axis;

  Region shapeBoundary;
  for (; ml; ml = ml->next) {
     vec2Add(&newPos, &ml->layer->posNext, &ml->velocity);
     abShapeGetBounds(ml->layer->abShape, &newPos, &shapeBoundary);
    for (axis = 0; axis < 2; axis ++) {
      if ((shapeBoundary.topLeft.axes[0] < fence->topLeft.axes[0]) ||
	 (shapeBoundary.botRight.axes[0] > fence->botRight.axes[0]) ){

	//shut sound off bit 6 for sound output
	P2SEL |= ~6;
	P2DIR = ~6;       
	clearScreen(COLOR_STEEL_BLUE);
	drawString5x7(30,50, "GAME OVER!!", COLOR_GREEN, COLOR_BLACK);
	drawString5x7(30,60, "GAME OVER!!", COLOR_RED, COLOR_BLACK);
	drawString5x7(30,70, "GAME OVER!!", COLOR_WHITE, COLOR_BLACK);
	drawString5x7(30,80, "GAME OVER!!", COLOR_YELLOW, COLOR_BLACK);
	drawString5x7(10,10, "GAME OVER!!", COLOR_GREEN, COLOR_RED);
       drawString5x7(10,140, "GAME OVER!!", COLOR_GREEN, COLOR_RED);
	//CPU off show light off
	or_sr(0x10);
	P1OUT &= ~GREEN_LED;

      }else if((shapeBoundary.topLeft.axes[1] < fence->topLeft.axes[1]) ||
	 (shapeBoundary.botRight.axes[1] > fence->botRight.axes[1]) ) {
	
	int velocity = ml->velocity.axes[axis] = -ml->velocity.axes[axis];
	 newPos.axes[axis] += (2*velocity);
        		    
      }else if(  (ball->pos.axes[0]-recRadius <= redPanel->pos.axes[0] + REC_WIDTH) &&
		 (ball->pos.axes[1] >= redPanel->pos.axes[1] - REC_LENGTH) &&
		 (ball->pos.axes[1] <= redPanel->pos.axes[1] + REC_LENGTH)  ||
		 (ball->pos.axes[0]+recRadius >= greenPanel->pos.axes[0] -REC_WIDTH) &&
		 (ball->pos.axes[1] <= greenPanel->pos.axes[1] + REC_LENGTH) &&
		 (ball->pos.axes[1] >= greenPanel->pos.axes[1] - REC_LENGTH)
		 /*    ||
		 //for easy game uncomment
		 (ship->pos.axes[0]-recRadius <= redPanel->pos.axes[0] + REC_WIDTH) &&
		 (ship->pos.axes[1] >= redPanel->pos.axes[1] - REC_LENGTH) &&
		 (ship->pos.axes[1] <= redPanel->pos.axes[1] + REC_LENGTH)  ||
		 (ship->pos.axes[0]+recRadius >= greenPanel->pos.axes[0] -REC_WIDTH) &&
		 (ship->pos.axes[1] <= greenPanel->pos.axes[1] + REC_LENGTH) &&
		 (ship->pos.axes[1] >= greenPanel->pos.axes[1] - REC_LENGTH) */){
	
	//set flag to give points
	addPoint = 1;
	//change velocity and direction
	int velocity = ml->velocity.axes[axis] = -ml->velocity.axes[axis];
	newPos.axes[axis] += (8*velocity);
        
      }//no collision
    } //for axis 
    ml->layer->posNext = newPos;
  } //for ml 
  
}

void manageButtonPress(int i, MovLayer *redLayer,MovLayer *greenLayer){
  int button1= 0;
  int button2 = 1;
  int button3 = 2;
  int button4 = 3;
  
  Vec2 newPosRed;
  Vec2 newPosGreen;
  
   vec2Add(&newPosRed, &redLayer->layer->posNext, &redLayer->velocity);
   vec2Add(&newPosGreen, &greenLayer->layer->posNext, &greenLayer->velocity);
    int velocity = redLayer->velocity.axes[1]  ;

     if(i == button1){
       //make going down sound
       buzzer_beat1();
       //move red panel down
       newPosRed.axes[1] += (velocity -10);
       redLayer->layer->posNext = newPosRed;
       //move green panel down
       newPosGreen.axes[1] += (velocity  -10);
       greenLayer->layer->posNext = newPosGreen;
  }else if( i == button2){
       //make going up sound
       buzzer_beat2();
       //move red panel up
       newPosRed.axes[1] += (velocity +10);
       redLayer->layer->posNext = newPosRed;
       //move green panel up
       newPosGreen.axes[1] += (velocity +10);
       greenLayer->layer->posNext = newPosGreen;
  }else if(i == button3){
       //make going left sound
       buzzer_beat3();
       //move green panel left
       newPosGreen.axes[0] += (velocity -10);
       greenLayer->layer->posNext = newPosGreen;
       //move red panel right
       newPosRed.axes[0] += (velocity +10);
       redLayer->layer->posNext = newPosRed;
  }else if( i == button4){
       //make going right sound
       buzzer_beat4();
       //move green panel right
       newPosGreen.axes[0] += (velocity +10);
       greenLayer->layer->posNext = newPosGreen;
       //move red panel left
       newPosRed.axes[0] += (velocity -10);
       redLayer->layer->posNext = newPosRed;
  }
    
  
}
//The background color
u_int bgColor = COLOR_BLACK;
//Boolean for whether screen needs to be redrawn
int redrawScreen = 1;           
//fence around playing field
Region fieldFence;		


/** Initializes everything, enables interrupts and green LED, 
 *  and handles the rendering for the screen
 */
void main(){

  //Green led on when CPU on 		
  P1DIR |= GREEN_LED;		
  P1OUT |= GREEN_LED;

  configureClocks();
  lcd_init();
  shapeInit();
  p2sw_init(15);

  shapeInit();
  buzzer_setup();
  layerInit(&layer0);
  layerDraw(&layer0);
  
  layerGetBounds(&fieldLayer, &fieldFence);

  //enable periodic interrupt
  enableWDTInterrupts();
  //GIE (enable interrupts)
  or_sr(0x8);	              
  int oneDigitPoints = 0;
  int twoDigitPoints = 0;
  int threeDIgitPoints = 0;
  
  //initialize scoreboard
   char strScore[4];
   u_int i ;
  for(i = 0; i < 4; i++){
     strScore[i] = '0';
  }
  
  for(;;){
    //increment the score points if flag is on
    if( addPoint == 1 && oneDigitPoints < 9){
      oneDigitPoints++;
      strScore[2] = '0'+oneDigitPoints;
      addPoint = 0;
    }else if(addPoint == 1 && oneDigitPoints == 9 && twoDigitPoints < 9) {
      oneDigitPoints = 0;
      twoDigitPoints++;
      strScore[2] = '0' + oneDigitPoints;
      strScore[1] = '0'+twoDigitPoints;
      addPoint = 0;
    }else if(addPoint == 1 && oneDigitPoints == 9 && twoDigitPoints == 9 && threeDIgitPoints < 9) {
      oneDigitPoints = 0;
      twoDigitPoints = 0;
      threeDIgitPoints++;
      strScore[2] = '0' + oneDigitPoints;
      strScore[1] = '0' + twoDigitPoints;
      strScore[0] = '0' + threeDIgitPoints;
      addPoint = 0;
    }

       
    strScore[3] = 0;
    drawString5x7( 90, 15, strScore, COLOR_BEIGE, COLOR_BLACK);
    drawString5x7( 45, 5, "YOUR SCORE: ", COLOR_GOLD, COLOR_BLACK);
    
    //Pause CPU if screen doesn't need updating 
    while (!redrawScreen) {
      //Green led off witHo CPU 
      P1OUT &= ~GREEN_LED;
      //CPU OFF 
      or_sr(0x10);	      
    }
   
    //Green led on when CPU on
    P1OUT |= GREEN_LED;       
    redrawScreen = 0;
    movLayerDraw(&ml0, &layer0);
  }//end of for
  
}

/** Watchdog timer interrupt handler. 15 interrupts/sec */
void wdt_c_handler(){
  
  static short count = 0;
  //Green LED on when cpu on
  P1OUT |= GREEN_LED;		      
  count ++;
  if (count == 15) {
    //moves shapes and implements colision effect
    checkForCollision(&layer0, &layer1, &layer3, &layer4, &ml0, &fieldFence);
    u_int switches = p2sw_read(),i;
    char str[5] ;
    for(i = 0; i < 4; i++){
      if( (switches & (1<<i)) ){	
	str[i] = '-';
      }else{	
	str[i] = '0'+i;
	//move panels as requested
	manageButtonPress(i, &ml1, &ml0);	 
      }
    }
    str[4]=0;
    drawString5x7(20,10, str, COLOR_YELLOW, COLOR_BLUE);
    redrawScreen = 1;
    count = 0;
  }
  //Green LED off when cpu off 
  P1OUT &= ~GREEN_LED;		    
}
