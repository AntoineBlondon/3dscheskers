#include <3ds.h>
#include <stdio.h>
#include <stdlib.h>
#include <citro2d.h>
#include <assert.h>
#include <string.h>
#include <time.h>


#define MAX_SPRITES   17
#define SCREEN_WIDTH  400
#define SCREEN_HEIGHT 240



typedef struct {
    char piece;
    int color;
    int selected;
} cell;

typedef struct {
    cell cells[8];
} line;

typedef struct {
    line lines[8];
} plateau;

typedef struct {
    int x;
    int y;
} coordinates;

typedef struct {
    int size;
    coordinates cells[20];
} moves;

typedef struct
{
	C2D_Sprite spr;
	coordinates pos;
} Sprite;

static C2D_SpriteSheet spriteSheet;
static Sprite sprites[MAX_SPRITES];
int HEIGHT = 31;
int WIDTH = 51;

int SELECTED_COLOR = 43;
int BACKGROUND_COLOR_1 = 42;
int BACKGROUND_COLOR_2 = 46;


coordinates add(coordinates a, coordinates b)
{
    coordinates c;
    c.x = a.x + b.x;
    c.y = a.y + b.y;
    return c;
}

static void initSprites()
{
	size_t numImages = C2D_SpriteSheetCount(spriteSheet);
	srand(time(NULL));
    int list[17] = {8, 0, 0, 1, 2, 3, 3, 3, 3, 4, 4, 5, 6, 7, 7, 7, 7};
	for (size_t i = 0; i < MAX_SPRITES; i++)
	{
		Sprite* sprite = &sprites[i];

		C2D_SpriteFromSheet(&sprite->spr, spriteSheet, list[i]);
		C2D_SpriteSetCenter(&sprite->spr, 0.5f, 0.5f);
		C2D_SpriteSetPos(&sprite->spr, 20 + 16*i, 20);
        coordinates co = {20 + 16*i, 20};
        sprite->pos = co;
	}
}

static void moveSprites() {
	for (size_t i = 0; i < MAX_SPRITES; i++)
	{
		Sprite* sprite = &sprites[i];
		C2D_SpriteSetPos(&sprite->spr, sprite->pos.x, sprite->pos.y);
	}
}

void setCoordinates(int x, int y)
{
    printf("\x1b[%i;%iH", x, y);
}
void setGlobalColor(int color)
{
    printf("\x1b[%im",color);
}

void setColor(int front, int background)
{
    printf("\x1b[%im\x1b[%im", front, background);
}

void setBackgroundColor(int color)
{
    for(int i = 0; i < HEIGHT; i++) {
        for(int j = 0; j < WIDTH; j++) {
            setCoordinates(i, j);
            setGlobalColor(44);
            printf(" ");
        }   
    }
}


void printEmptyPlateau()
{
    for(int i = (HEIGHT+1)/2 - 4; i < (HEIGHT+1)/2 + 4; i++) {
        for(int j = (WIDTH+1)/2 - 4; j < (WIDTH+1)/2 + 4; j++) {
            setCoordinates(i, j);
            setGlobalColor( (i+j)%2 == 0 ? 42 : 46);
            printf(" ");
        }   
    }
}


void goTo(int x, int y)
{
    setCoordinates(((HEIGHT+1)/2 - 4)+x, ((WIDTH+1)/2 -4)+y);
}
void setBasicBackground(int x, int y) {
    goTo(x,y);
    setGlobalColor( (x+y)%2 == 0 ? 42 : 46);
}

void printBorders()
{
    goTo(-1,0);
    setColor(37,45);
    printf("01234567");
    for(int i = 0; i < 8; i++) {
        goTo(i,-1);
        printf("%i",i);
    }
}

plateau emptyPlateau()
{
    plateau pl;

    line l;

    cell c;
    c.piece = ' ';
    c.color = 0;
    c.selected = 0;

    for(int i = 0; i < 8; i++) {
        l.cells[i] = c;
    }
    for(int i = 0; i < 8; i++) {
        pl.lines[i] = l;
    }
    


    return pl;
}

void printPlateau(plateau pl)
{
    for(int i = 0; i < 8; i++) {
        line l = pl.lines[i];
        for(int j = 0; j < 8; j++) {
            cell c = l.cells[j];
            goTo(j, i);
            setColor(c.color == 1 ? 30 : 37, c.selected == 1 ? SELECTED_COLOR :((i+j)%2 == 0 ? BACKGROUND_COLOR_1 : BACKGROUND_COLOR_2));
            printf("%c",c.piece);
        }
    }
}

plateau *setCell(plateau *pl, cell c, int x, int y)
{
    pl->lines[x].cells[y] = c;
    return pl;
}
plateau *moveCell(plateau *pl, int dx, int dy, int nx, int ny)
{
    cell c = pl->lines[dx].cells[dy];
    cell b = {' ', 0, 0};

    pl = setCell(pl, c, nx, ny);
    pl = setCell(pl, b, dx, dy);

    return pl;
}

plateau startPlateau() {
    plateau pl = emptyPlateau();
    cell z1 = {'z', 1, 0};
    cell r1 = {'r', 1, 0};
    cell f1 = {'f', 1, 0};
    cell p1 = {'p', 1, 0};

    pl = *setCell(&pl, z1, 1, 0);
    pl = *setCell(&pl, r1, 3, 0);
    pl = *setCell(&pl, r1, 5, 0);
    pl = *setCell(&pl, f1, 7, 0);
    pl = *setCell(&pl, p1, 0, 1);
    pl = *setCell(&pl, p1, 2, 1);
    pl = *setCell(&pl, p1, 4, 1);
    pl = *setCell(&pl, p1, 6, 1);

    cell z2 = {'z', 0, 0};
    cell r2 = {'r', 0, 0};
    cell f2 = {'f', 0, 0};
    cell p2 = {'p', 0, 0};

    pl = *setCell(&pl, z2, 6, 7);
    pl = *setCell(&pl, r2, 4, 7);
    pl = *setCell(&pl, r2, 2, 7);
    pl = *setCell(&pl, f2, 0, 7);
    pl = *setCell(&pl, p2, 1, 6);
    pl = *setCell(&pl, p2, 3, 6);
    pl = *setCell(&pl, p2, 5, 6);
    pl = *setCell(&pl, p2, 7, 6);
    return pl;
}

int isValid(int x, int y)
{
    return x >= 0 && x < 8 && y >= 0 && y < 8;
}

int testPawn(int x, int y, plateau pl)
{
    return isValid(x,y) && pl.lines[x].cells[y].piece == ' ';
}

int testZebre(plateau pl, coordinates pos, cell c)
{
    int x = pos.x;
    int y = pos.y;
    return isValid(x, y) && (pl.lines[x].cells[y].piece == ' ' || pl.lines[x].cells[y].color != c.color);
}
int testFou(plateau pl, coordinates pos, cell c)
{
    int x = pos.x;
    int y = pos.y;
    return isValid(x, y) && (pl.lines[x].cells[y].piece == ' ' || pl.lines[x].cells[y].color != c.color);
}

int canSauteMouton(plateau pl, coordinates debut, coordinates fin)
{
    coordinates between = {(debut.x+fin.x)/2, (debut.y+fin.y)/2};
    return isValid(fin.x, fin.y) && pl.lines[fin.x].cells[fin.y].piece == ' ' && pl.lines[between.x].cells[between.y].piece != ' ' && pl.lines[between.x].cells[between.y].color != pl.lines[debut.x].cells[debut.y].color;
}

moves getMovesOfPion(plateau pl, int x, int y)
{
    cell c = pl.lines[x].cells[y];
    moves m;
    m.size = 0;

    switch(c.color) {
        case 0:
            if(testPawn(x+1,y-1, pl)) {
                coordinates a = {x+1, y-1};
                m.size++;
                m.cells[m.size-1] = a;
            }
            if(testPawn(x-1,y-1, pl)) {
                coordinates b = {x-1, y-1};
                m.size++;
                m.cells[m.size-1] = b;
            }
    
            break;
        case 1:
            if(testPawn(x+1,y+1, pl)) {
                coordinates a = {x+1, y+1};
                m.size++;
                m.cells[m.size-1] = a;
            }
            if(testPawn(x-1,y+1, pl)) {
                coordinates b = {x-1, y+1};
                m.size++;
                m.cells[m.size-1] = b;
            }
    
            break;
    }

    coordinates co1 = {x+2,y+2};
    coordinates co2 = {x-2,y+2};
    coordinates co3 = {x+2,y-2};
    coordinates co4 = {x-2,y-2};
    

    coordinates cs[] = {co1, co2, co3, co4};

    for(int i = 0; i < 4; i++ ){
        coordinates co = cs[i];
        coordinates debut = {x, y};
        
        if(canSauteMouton(pl, debut, co)){

            m.size++;
            m.cells[m.size-1] = co;
        }

    }



    return m;
    
}

moves getMovesToEat(plateau pl, int x, int y)
{
    moves m;
    m.size = 0;
    coordinates co1 = {x+2,y+2};
    coordinates co2 = {x-2,y+2};
    coordinates co3 = {x+2,y-2};
    coordinates co4 = {x-2,y-2};
    

    coordinates cs[] = {co1, co2, co3, co4};

    for(int i = 0; i < 4; i++ ){
        coordinates co = cs[i];
        coordinates debut = {x, y};
        
        if(canSauteMouton(pl, debut, co)){

            m.size++;
            m.cells[m.size-1] = co;
        }

    }



    return m;
}
moves getMovesOfRoi(plateau pl, int x, int y)
{
    moves m;
    m.size = 0;

    
     
    if(testPawn(x+1,y-1, pl)) {
        coordinates a = {x+1, y-1};
        m.size++;
        m.cells[m.size-1] = a;
    }
    if(testPawn(x-1,y-1, pl)) {
        coordinates b = {x-1, y-1};
        m.size++;
        m.cells[m.size-1] = b;
    }



    if(testPawn(x+1,y+1, pl)) {
        coordinates c = {x+1, y+1};
        m.size++;
        m.cells[m.size-1] = c;
    }
    if(testPawn(x-1,y+1, pl)) {
        coordinates d = {x-1, y+1};
        m.size++;
        m.cells[m.size-1] = d;
    }


    

    coordinates co1 = {x-2,y-2};
    coordinates co2 = {x-2,y+2};
    coordinates co3 = {x+2,y-2};
    coordinates co4 = {x+2,y+2};
    

    coordinates cs[] = {co1, co2, co3, co4};

    for(int i = 0; i < 4; i++ ){
        coordinates co = cs[i];
        coordinates debut = {x, y};
        if(canSauteMouton(pl, debut, co)){
            m.size++;
            m.cells[m.size-1] = co;
        }

    }



    return m;
}

moves getMovesOfZebre(plateau pl, int x, int y)
{
    moves m;
    m.size = 0;

    coordinates co1 = {x+1,y+3};
    coordinates co2 = {x-1,y+3};
    coordinates co3 = {x+1,y-3};
    coordinates co4 = {x-1,y-3};
    coordinates co5 = {x+3,y+1};
    coordinates co6 = {x-3,y+1};
    coordinates co7 = {x+3,y-1};
    coordinates co8 = {x-3,y-1};
    

    coordinates cs[] = {co1, co2, co3, co4, co5, co6, co7, co8};

    for(int i = 0; i < 8; i++ ){
        coordinates co = cs[i];
        if(testZebre(pl, co, pl.lines[x].cells[y])){
            m.size++;
            m.cells[m.size-1] = co;
        }

    }


    return m;
}

moves getMovesOfFou(plateau pl, int x, int y)
{
    moves m;
    m.size = 0;

    coordinates co1 = {1,1};
    coordinates co2 = {-1,1};
    coordinates co3 = {1,-1};
    coordinates co4 = {-1,-1};
    

    coordinates cs[] = {co1, co2, co3, co4};

    for(int i = 0; i < 4; i++ ){
        coordinates toAdd = cs[i];
        coordinates current = {x, y};

        while(isValid(add(current,toAdd).x, add(current,toAdd).y)) {
            current = add(current,toAdd);
            
            if(testFou(pl, current, pl.lines[x].cells[y])){
                m.size++;
                m.cells[m.size-1] = current;
            }
            if(pl.lines[current.x].cells[current.y].piece != ' ') {
                break;   
            }
        }
        

    }


    return m;
}

moves getPossibleMoves(plateau pl, int x, int y)
{
    cell c = pl.lines[x].cells[y];
    moves m;
    m.size = 0;

    switch(c.piece) {
        case 'p':
            m = getMovesOfPion(pl, x, y);
            break;
        case 'r':
            m = getMovesOfRoi(pl, x, y);
            break;
        case 'z':
            m = getMovesOfZebre(pl, x, y);
            break;
        case 'f':
            m = getMovesOfFou(pl, x, y);
            break;
    }
    return m;
}

plateau *selectCells(plateau *pl, moves cells)
{
    for(int i = 0; i < cells.size; i++) {
        pl->lines[cells.cells[i].x].cells[cells.cells[i].y].selected = 1;
    }
    return pl;
}

plateau *clearSelection(plateau *pl)
{
     for(int i = 0; i < 8; i++) {
        for(int j = 0; j < 8; j++) {
            pl->lines[i].cells[j].selected = 0;
        }
    }
    return pl;
}

int isValidMove(plateau pl, coordinates debut, coordinates fin)
{
    moves m = getPossibleMoves(pl, debut.x, debut.y);

    for(int i = 0; i < m.size; i++) {
        coordinates move = m.cells[i];

        if(move.x == fin.x && move.y == fin.y) {
            return 1;
        }
    }
    return 0;
}

int isEatingMove(cell c, coordinates start, coordinates fin)
{
    return (c.piece == 'r' || c.piece == 'p') && abs(start.x-fin.x) == 2 && abs(start.y-fin.y) == 2; 
}

plateau *correctMove(plateau *pl, coordinates debut, coordinates fin)
{
    if(isValidMove(*pl, debut, fin)) {
        pl = moveCell(pl, debut.x, debut.y, fin.x, fin.y);
    }
    return pl;
}

plateau testPlateau()
{
    plateau pl = startPlateau();

    pl = *moveCell(&pl, 2, 1, 2, 5);
    pl = *moveCell(&pl, 0, 1, 2, 3);
    return pl;
}

cell getCell(plateau pl, coordinates co)
{
    return pl.lines[co.x].cells[co.y];
}

plateau *destroy(plateau *pl, coordinates selected, coordinates e)
{
    coordinates between = {(selected.x+e.x)/2, (selected.y+e.y)/2};
    cell ce = {' ', 0, 0};

    pl = setCell(pl, ce, between.x, between.y);
    return pl;
}


int main()
{
    // Init libs
	romfsInit();
	gfxInitDefault();
	C3D_Init(C3D_DEFAULT_CMDBUF_SIZE);
	C2D_Init(C2D_DEFAULT_MAX_OBJECTS);
	C2D_Prepare();
	consoleInit(GFX_BOTTOM, NULL);

	// Create screens
	C3D_RenderTarget* top = C2D_CreateScreenTarget(GFX_TOP, GFX_LEFT);

	// Load graphics
	spriteSheet = C2D_SpriteSheetLoad("romfs:/gfx/sprites.t3x");
	if (!spriteSheet) svcBreak(USERBREAK_PANIC);

	// Initialize sprites
	initSprites();

	plateau pl = startPlateau();
    int turn = 0;
    int needToEat = 0;
    coordinates selected = {-1,-1};

    setBackgroundColor(44);
    setCoordinates(0, 1);
    printf("%i", C2D_SpriteSheetCount(spriteSheet));
    


    // Main loop
    while (aptMainLoop()) {
        
        
        hidScanInput();

		// Respond to user input
		u32 kDown = hidKeysDown();
		if (kDown & KEY_START)
			break; // break in order to return to hbmenu

        moveSprites();
        C3D_FrameBegin(C3D_FRAME_SYNCDRAW);
        C2D_TargetClear(top, C2D_Color32f(0.0f, 0.0f, 0.0f, 1.0f));
        C2D_SceneBegin(top);
        for (size_t i = 0; i < MAX_SPRITES; i ++)
            C2D_DrawSprite(&sprites[i].spr);
        C3D_FrameEnd(0);
        

        setCoordinates(0,0);
        setColor((turn==1) ? 30 : 37,44);
        printf("Au tour du joueur ");
        printf("%i", turn + 1);

        pl = *clearSelection(&pl);
        

        if(selected.x != -1) {
            if(needToEat) {
                pl = *selectCells(&pl, getMovesToEat(pl, selected.x, selected.y));
            } else {
                pl = *selectCells(&pl, getPossibleMoves(pl, selected.x, selected.y));
            }
        }

        

        
        

        printPlateau(pl);
        
        

        static SwkbdState swkbd;
		static char mybuf[60];
		static SwkbdStatusData swkbdStatus;
		static SwkbdLearningData swkbdLearning;
		SwkbdButton button = SWKBD_BUTTON_NONE;
		bool didit = false;


		if (kDown & KEY_B)
		{
			didit = true;
			swkbdInit(&swkbd, SWKBD_TYPE_NUMPAD, 1, 8);
			swkbdSetPasswordMode(&swkbd, SWKBD_PASSWORD_HIDE_DELAY);
			swkbdSetValidation(&swkbd, SWKBD_ANYTHING, 0, 0);
			swkbdSetFeatures(&swkbd, SWKBD_FIXED_WIDTH);
			swkbdSetNumpadKeys(&swkbd, L'ツ', L'益');
			button = swkbdInputText(&swkbd, mybuf, sizeof(mybuf));
		}

        if (didit && button != SWKBD_BUTTON_NONE)
		{
			
            int xi = mybuf[0]-48;
            int yi = mybuf[1]-48;
            
            
            cell c = pl.lines[xi].cells[yi];

            if(needToEat) {
                if(c.piece == ' ') {
                    moves m = getMovesToEat(pl, selected.x, selected.y);
                    for(int i = 0; i < m.size; i++) {
                        coordinates e = m.cells[i];

                        if(e.x == xi && e.y == yi) {
                            pl = *correctMove(&pl, selected, e);
                            
                            pl = *destroy(&pl, selected, e);
                            if(getMovesToEat(pl, e.x,e.y).size == 0) {
                                needToEat = 0;
                            }
                        }
                    }
                    if(!needToEat) {
                        coordinates co = {-1, -1};
                        selected = co;
                        turn = (turn == 1) ? 0 : 1;
                    }
                }
                continue;
            }


        
            int could = 0;


            if(c.piece != ' ' && c.color == turn) {
                coordinates co = {xi, yi};
                selected = co;
            }

            if(c.piece == ' ') {

            
                moves m = getPossibleMoves(pl, selected.x, selected.y);


                for(int i = 0; i < m.size; i++) {
                    coordinates e = m.cells[i];

                    if(e.x == xi && e.y == yi) {
                        pl = *correctMove(&pl, selected, e);
                        if(isEatingMove(pl.lines[e.x].cells[e.y], selected, e)) {
                            pl = *destroy(&pl, selected, e);
                            if(getMovesToEat(pl, e.x,e.y).size > 0) {
                                needToEat = 1;
                                selected = e;
                            }
                        }
                        could = 1;
                    }
                }

                if(!needToEat) {
                    coordinates co = {-1, -1};
                    selected = co;
                }
                
                
            }

            if(c.piece != ' ' && c.color != turn) {
                moves m = getPossibleMoves(pl, selected.x, selected.y);

                for(int i = 0; i < m.size; i++) {
                    coordinates e = m.cells[i];

                    if(e.x == xi && e.y == yi) {
                        pl = *correctMove(&pl, selected, e);
                        could = 1;
                    }
                }
                coordinates co = {-1, -1};
                selected = co;

            }

            if(could) turn = (turn == 1) ? 0 : 1;
		}

        
        
        
        
		//Wait for VBlank
		gspWaitForVBlank();
    }

	// Delete graphics
	C2D_SpriteSheetFree(spriteSheet);

	// Deinit libs
	C2D_Fini();
	C3D_Fini();
	gfxExit();
	romfsExit();
	return 0;
}





// 0 -> selected
// 0 -> color
// 0 |
// 0 |-> piece 
// 0 |

// 5-bit number


// 00 -> none
// 01 -> pawn
// 10 -> king
// 11 -> bishop
// 100 -> zebra
//
// 000  3 bits for type of piece
// 0 1 bit for color
// 0 1 bit for selected