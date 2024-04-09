#include "raylib.h"
#include <stdio.h>
#include <stdlib.h>


const int screenWidth = 700;
const int screenHeight = 700;
const int squareSize = 4;
const int UISquareSize = 20;
const int numberOfBlocks = 10;
const int FPS = 125;
const int placeBlocksDistance = squareSize*5;
const Vector2 gridSize = {screenWidth/squareSize, screenHeight/squareSize};
const int size = (screenWidth/squareSize) * (screenHeight/squareSize);
const Color emptyColor = WHITE;
const Color backgroundColor = {100, 100, 100};


typedef struct {
    Vector2 pos;
    int blockType;
    Color color;
    bool canDestroy;
    int lifetime;
}block, blockUI;

typedef struct {
    int block1Type;
    int arrayBlock1Place;
    Color color1;
    int block2Type;
    int arrayBlock2Place;
    Color color2;
    bool changeThird;
    int block3Type;
    int arrayBlock3Place;
    Color color3;
}blockReturner;

void InitializeGame(block grid[], blockUI gridUI[]);
static void PlaceBlocks(block grid[], int currentMouseBlock, blockUI gridUI[], int *changeMouseBlock);
static void FrameUpdate(block grid[]);
static void UpdateDrawFrame(block grid[], blockUI gridUI[], int currentBlock);
blockReturner SandToFall(int cord, block grid[]);
blockReturner WaterToFall(int cord, block grid[], int waterVel);
blockReturner GetBlockUnder(block grid[], int currentBlock, int nextBlock);
blockReturner GetBlockUnderForWater(block grid[], int currentBlock, int nextBlock);
blockReturner GetBlocksForFire(block grid[], int currentBlock, int nextBlock);
blockReturner SmokeToFly(int cord, block grid[], int waterVel);
blockReturner AcidToFall(int cord, block grid[], int waterVel);
blockReturner ExplosiveToFall(int cord, block grid[]);
blockReturner GetBlockUnderExplosive(block grid[], int currentBlock, int nextBlock);
blockReturner GetBlockUnderForAcid(block grid[], int currentBlock, int nextBlock);
blockReturner MaintainBlock(block grid[], int currentBlock, int nextBlock);
void CycleTheGridDownUp(block grid[]);
void CycleTheGridUpDown(block grid[]);
bool BlockChecker(block grid[], int i);
bool checkBlocksForAcid(int i);
Color Colors(int i);
void RestartAllBlocks(block grid[]);
char *Name(int i);
block ReturnBlock(Vector2 pos, int type)
{
    block b = {pos, type, Colors(type)};
    return b;
}

int main()
{
    InitWindow(screenWidth, screenHeight, "Sand Falling Simulator");
    SetTargetFPS(FPS);

    
    block grid[size];
    blockUI gridUI[numberOfBlocks];
    int currentMouseBlock = 1;
    InitializeGame(grid, gridUI);


    
    while (!WindowShouldClose())
    {
        PlaceBlocks(grid, currentMouseBlock, gridUI, &currentMouseBlock);
        UpdateDrawFrame(grid, gridUI, currentMouseBlock);
        FrameUpdate(grid);
        if(IsKeyPressed(KEY_R))
            RestartAllBlocks(grid);
    }

    CloseWindow();
    
    return 0;
}


void InitializeGame(block grid[], blockUI gridUI[])
{
    Vector2 lastPos = (Vector2) {0, 0};
    for(int i = 0; i<size; i++)
    {
        grid[i].pos = lastPos;
        grid[i].blockType = 0;
        grid[i].color = Colors(0);
        if(lastPos.x >= gridSize.x-1)
        {
            lastPos.x = 0;
            lastPos.y++;            
        }else
        {
            lastPos.x++;
        }
    }
    int cellSizeForUI = screenHeight/numberOfBlocks;
    for (int i = 0; i < numberOfBlocks; i++)
    {
        Vector2 posToGive = {30 + 30*i, 25};
        gridUI[i].pos = posToGive;
        gridUI[i].blockType = i+1;
        gridUI[i].color = Colors(i+1);
    }
}


static void PlaceBlocks(block grid[], int currentMouseBlock, blockUI gridUI[], int *changeMouseBlock)
{
    bool onTopOfUI = false;
    int checkAllUI;
    for(int i=0; i < numberOfBlocks; i++)
    {
        Vector2 vector = {gridUI[i].pos.x - GetMousePosition().x, gridUI[i].pos.y - GetMousePosition().y};
        if(abs(vector.x) < UISquareSize && abs(vector.y) < UISquareSize && IsMouseButtonDown(MOUSE_BUTTON_LEFT))
        {
            *changeMouseBlock = gridUI[i].blockType;
            onTopOfUI = true;
        }else
        {
            checkAllUI++;
        }
    }
    if(checkAllUI == numberOfBlocks)
    {
        onTopOfUI = false;
    }
    checkAllUI = 0;
    for(int i = 0; i<size; i++)
    {
        Vector2 vectorToMouse = {(grid[i].pos.x * squareSize + squareSize/2) - GetMousePosition().x, (grid[i].pos.y * squareSize + squareSize/2) - GetMousePosition().y};
        if(abs(vectorToMouse.x) < placeBlocksDistance && abs(vectorToMouse.y) < placeBlocksDistance && IsMouseButtonDown(MOUSE_BUTTON_LEFT) && !onTopOfUI)
        {
            if(currentMouseBlock == 7)
            {
                if(GetRandomValue(0, 1) == 1)
                {
                    grid[i].blockType = currentMouseBlock;
                    grid[i].color = Colors(currentMouseBlock);
                }
            }else
            {
                grid[i].blockType = currentMouseBlock;
                grid[i].color = Colors(currentMouseBlock);
            }
        }else if(abs(vectorToMouse.x) < placeBlocksDistance && abs(vectorToMouse.y) < placeBlocksDistance && IsMouseButtonDown(MOUSE_BUTTON_RIGHT))
        {
            grid[i].blockType = 0;
            grid[i].color = Colors(0);
        }
    }
}



static void FrameUpdate(block grid[])
{
    CycleTheGridDownUp(grid);
    CycleTheGridUpDown(grid);
}

void CycleTheGridDownUp(block grid[])
{
    for(int i = size; i>-1; i--)
    {
        blockReturner block;
        switch (grid[i].blockType)
        {
            case 1:
                block = SandToFall(i, grid);
                grid[block.arrayBlock1Place].blockType = block.block1Type;
                grid[block.arrayBlock1Place].color = block.color1;
                grid[block.arrayBlock2Place].blockType = block.block2Type;
                grid[block.arrayBlock2Place].color = block.color2;
                if(block.changeThird)
                    grid[block.arrayBlock3Place].blockType = block.block3Type;
                break;
            case 2:
                block = WaterToFall(i, grid, 3);
                grid[block.arrayBlock1Place].blockType = block.block1Type;
                grid[block.arrayBlock1Place].color = Colors(block.block1Type);
                grid[block.arrayBlock2Place].blockType = block.block2Type;
                grid[block.arrayBlock2Place].color = Colors(block.block2Type);
                break;
            case 4:
                block = WaterToFall(i, grid, 1);
                grid[block.arrayBlock1Place].blockType = block.block1Type;
                grid[block.arrayBlock1Place].color = Colors(block.block1Type);
                grid[block.arrayBlock2Place].blockType = block.block2Type;
                grid[block.arrayBlock2Place].color = Colors(block.block2Type);
                break;
            case 6:
                block = SandToFall(i, grid);
                grid[block.arrayBlock1Place].blockType = block.block1Type;
                grid[block.arrayBlock1Place].color = block.color1;
                grid[block.arrayBlock2Place].blockType = block.block2Type;
                grid[block.arrayBlock2Place].color = block.color2;
                if(block.changeThird)
                    grid[block.arrayBlock3Place].blockType = block.block3Type;
                break;
            case 7:
                int blockToCheck = i+1;
                if(blockToCheck <= size && blockToCheck > 0)
                {
                    block = GetBlocksForFire(grid, i, blockToCheck);
                    grid[block.arrayBlock2Place].blockType = block.block2Type;
                    grid[block.arrayBlock2Place].color = Colors(block.block2Type);
                }
                blockToCheck = i+gridSize.x;
                if(blockToCheck <= size && blockToCheck > 0)
                {
                    block = GetBlocksForFire(grid, i, blockToCheck);
                    grid[block.arrayBlock2Place].blockType = block.block2Type;
                    grid[block.arrayBlock2Place].color = Colors(block.block2Type);
                }

                if(!grid[i].canDestroy) grid[i].canDestroy = true;
                else
                {
                    if(GetRandomValue(1, 10) == 1)
                    {
                        grid[i].blockType = 8;
                        grid[i].color = Colors(8);
                    }else
                    {
                        grid[i].blockType = 0;
                        grid[i].color = Colors(0);
                    }
                    grid[i].canDestroy = false;
                }
                break;
            case 9:
                block = AcidToFall(i, grid, 15);
                grid[block.arrayBlock1Place].blockType = block.block1Type;
                grid[block.arrayBlock1Place].color = Colors(block.block1Type);
                grid[block.arrayBlock2Place].blockType = block.block2Type;
                grid[block.arrayBlock2Place].color = Colors(block.block2Type);
                break;
            case 10:
                block = ExplosiveToFall(i, grid);
                grid[block.arrayBlock1Place].blockType = block.block1Type;
                grid[block.arrayBlock1Place].color = block.color1;
                grid[block.arrayBlock2Place].blockType = block.block2Type;
                grid[block.arrayBlock2Place].color = block.color2;
                if(block.changeThird)
                    grid[block.arrayBlock3Place].blockType = block.block3Type;
                break;
        }
    }
}

void CycleTheGridUpDown(block grid[])
{
    for(int i = 0; i<=size; i++)
    {
        blockReturner block;
        switch (grid[i].blockType)
        {
            case 7:
                int blockToCheck = i-1;
                if(blockToCheck <= size && blockToCheck > 0)
                {
                    block = GetBlocksForFire(grid, i, blockToCheck);
                    grid[block.arrayBlock2Place].blockType = block.block2Type;
                    grid[block.arrayBlock2Place].color = Colors(block.block2Type);
                }
                blockToCheck = i-gridSize.x;
                if(blockToCheck <= size && blockToCheck > 0)
                {
                    block = GetBlocksForFire(grid, i, blockToCheck);
                    grid[block.arrayBlock2Place].blockType = block.block2Type;
                    grid[block.arrayBlock2Place].color = Colors(block.block2Type);
                }

                if(!grid[i].canDestroy) grid[i].canDestroy = true;
                else
                {
                    if(GetRandomValue(1, 10) == 1)
                    {
                        grid[i].blockType = 8;
                        grid[i].color = Colors(8);
                    }else
                    {
                        grid[i].blockType = 0;
                        grid[i].color = Colors(0);
                    }
                }

                break;
            case 11:
            case 8:
                block = SmokeToFly(i, grid, 5);
                grid[block.arrayBlock1Place].blockType = block.block1Type;
                grid[block.arrayBlock1Place].color = Colors(block.block1Type);
                grid[block.arrayBlock2Place].blockType = block.block2Type;
                grid[block.arrayBlock2Place].color = Colors(block.block2Type);
                if(grid[i].blockType == 11)
                {
                    if(GetRandomValue(1, 75) == 1)
                    {
                        grid[i].blockType = 8;
                        grid[i].color = Colors(8);
                    }
                }

                if(grid[i].lifetime < GetRandomValue(0, 2000)) grid[i].lifetime++;
                else {
                    grid[i].lifetime = 0; 
                    grid[i].blockType = 0;
                    grid[i].color = Colors(0);
                }

                break;
        }
    }
}


static void UpdateDrawFrame(block grid[], blockUI gridUI[], int currentBlock)
{


    BeginDrawing();

        ClearBackground(backgroundColor);

        for(int i = 0; i<size; i++)
        {
            DrawRectangle(grid[i].pos.x * squareSize, grid[i].pos.y * squareSize, squareSize, squareSize, grid[i].color);
        }

        for(int i = 0; i<numberOfBlocks; i++)
        {
            DrawRectangle(gridUI[i].pos.x - (UISquareSize/2), gridUI[i].pos.y - (UISquareSize/2), UISquareSize, UISquareSize, gridUI[i].color);
        }
        DrawText(Name(currentBlock), 15 + 30 * numberOfBlocks, 15, 25, BLACK);
        DrawFPS(screenWidth-100, 10);

    EndDrawing();
}


blockReturner SandToFall(int cord, block grid[])
{
    int blockUnder = cord + gridSize.x;
    if(blockUnder <= size){
        if(BlockChecker(grid, blockUnder)) return GetBlockUnder(grid, cord, blockUnder);

        int randValue = GetRandomValue(1, 2);
        if(randValue == 1)
        {
            if(BlockChecker(grid, blockUnder-1)) return GetBlockUnder(grid, cord, blockUnder-1);
        }else
        {
            if(BlockChecker(grid, blockUnder+1)) return GetBlockUnder(grid, cord, blockUnder+1);
        }
    }
    return MaintainBlock(grid, cord, cord);
}

blockReturner WaterToFall(int cord, block grid[], int waterVel)
{
    int blockUnder = cord + gridSize.x;
    if(blockUnder <= size){
        if(grid[blockUnder].blockType == 0 || grid[blockUnder].blockType == 9 || (grid[blockUnder].blockType == 4 && grid[cord].blockType == 2) || (grid[blockUnder].blockType == 2 && grid[cord].blockType == 4) || (grid[blockUnder].blockType == 6 && grid[cord].blockType == 4) || (grid[blockUnder].blockType == 3 && grid[cord].blockType == 4) || (grid[blockUnder].blockType == 8 && grid[cord].blockType == 2) || (grid[blockUnder].blockType == 8 && grid[cord].blockType == 4) || (grid[blockUnder].blockType == 11 && grid[cord].blockType == 4) || (grid[blockUnder].blockType == 11 && grid[cord].blockType == 2)) return GetBlockUnderForWater(grid, cord, blockUnder);

        int randValue = GetRandomValue(1, 2);
        if(randValue == 1)
        {
            int leftSize = 1;
            for(int i = 2; i<=waterVel; i++)
            {
                if (grid[cord-i].blockType == 0)
                {
                    leftSize++;
                }else break;
            }
            if(grid[cord-leftSize].blockType == 0 || grid[cord+leftSize].blockType == 9 || (grid[cord+leftSize].blockType == 4 && grid[cord].blockType == 2) || (grid[cord+leftSize].blockType == 2 && grid[cord].blockType == 4) || (grid[cord+leftSize].blockType == 6 && grid[cord].blockType == 4) || (grid[cord+leftSize].blockType == 3 && grid[cord].blockType == 4) || (grid[cord+leftSize].blockType == 8 && grid[cord].blockType == 2) || (grid[cord+leftSize].blockType == 8 && grid[cord].blockType == 4) || (grid[cord+leftSize].blockType == 11 && grid[cord].blockType == 4) || (grid[cord+leftSize].blockType == 11 && grid[cord].blockType == 2)) return GetBlockUnderForWater(grid, cord, cord-leftSize);
        }else if(randValue == 2)
        {
            int rightSize = 1;
            for(int i = 2; i<=waterVel; i++)
            {
                if (grid[cord+i].blockType == 0)
                {
                    rightSize++;
                }else break;
            }
            if(grid[cord+rightSize].blockType == 0 || grid[cord+rightSize].blockType == 9 || (grid[cord+rightSize].blockType == 4 && grid[cord].blockType == 2) || (grid[cord+rightSize].blockType == 2 && grid[cord].blockType == 4) || (grid[cord+rightSize].blockType == 6 && grid[cord].blockType == 4) || (grid[cord+rightSize].blockType == 3 && grid[cord].blockType == 4) || (grid[cord+rightSize].blockType == 8 && grid[cord].blockType == 4) || (grid[cord+rightSize].blockType == 8 && grid[cord].blockType == 2)) return GetBlockUnderForWater(grid, cord, cord+rightSize);
        }
    }
    return MaintainBlock(grid, cord, cord);
}

blockReturner AcidToFall(int cord, block grid[], int waterVel)
{
    int blockUnder = cord + gridSize.x;
    if(blockUnder <= size){
        if(checkBlocksForAcid(grid[blockUnder].blockType)) return GetBlockUnderForAcid(grid, cord, blockUnder);

        int randValue = GetRandomValue(1, 2);
        if(randValue == 1)
        {
            int leftSize = GetRandomValue(0, waterVel);
            for(int i = 1; i<=leftSize; i++)
            {
                if (!checkBlocksForAcid(grid[cord-i].blockType))
                {
                    leftSize = i;
                    break;
                }
            }
            if(checkBlocksForAcid(grid[cord-leftSize].blockType)) return GetBlockUnderForAcid(grid, cord, cord-leftSize);
        }else if(randValue == 2)
        {
            int rightSize = GetRandomValue(0, waterVel);
            for(int i = 1; i<=rightSize; i++)
            {
                if (!checkBlocksForAcid(grid[cord+i].blockType))
                {
                    rightSize = i;
                    break;
                }
            }
            if(checkBlocksForAcid(grid[cord+rightSize].blockType)) return GetBlockUnderForAcid(grid, cord, cord+rightSize);
        }
    }
    return MaintainBlock(grid, cord, cord);
}

blockReturner SmokeToFly(int cord, block grid[], int waterVel)
{
    int blockAbove = cord - gridSize.x;
    if(blockAbove >= 0){
        if(grid[blockAbove].blockType == 0) return GetBlockUnderForWater(grid, cord, blockAbove);

        int randValue = GetRandomValue(1, 2);
        if(randValue == 1)
        {
            int leftSize = 1;
            for(int i = 2; i<=waterVel; i++)
            {
                if (grid[cord-i].blockType == 0)
                {
                    leftSize++;
                }else break;
            }
            if(grid[cord-leftSize].blockType == 0) return GetBlockUnderForWater(grid, cord, cord-leftSize);
        }else if(randValue == 2)
        {
            int rightSize = 1;
            for(int i = 2; i<=waterVel; i++)
            {
                if (grid[cord+i].blockType == 0)
                {
                    rightSize++;
                }else break;
            }
            if(grid[cord+rightSize].blockType == 0) return GetBlockUnderForWater(grid, cord, cord+rightSize);
        }
    }
    return MaintainBlock(grid, cord, cord);
}

void DestroyBlocks(block grid[], Vector2 pos)
{
    for(int i = size; i>-1; i--)
    {
        Vector2 vec = {grid[i].pos.x-pos.x, grid[i].pos.y-pos.y};
        if(grid[i].blockType != 5 && vec.x*vec.x + vec.y*vec.y < squareSize*250)
        {
            int rand = GetRandomValue(1, 3);
            if(rand == 1 || rand == 2 || grid[i].blockType == 10)
            {
                grid[i].blockType = 11;
                grid[i].color = Colors(11);
            }
        }
    }
}

blockReturner ExplosiveToFall(int cord, block grid[])
{
    int blockUnder = cord + gridSize.x;
    if(blockUnder <= size){
        if(BlockChecker(grid, blockUnder)) return GetBlockUnder(grid, cord, blockUnder);
        DestroyBlocks(grid, grid[cord].pos);
    }
    return MaintainBlock(grid, cord, cord);
}


blockReturner GetBlockUnder(block grid[], int currentBlock, int nextBlock)
{
    int block1Type = grid[currentBlock].blockType;
    int block2Type = grid[nextBlock].blockType;
    bool changeThird;
    int block3Type;
    int lastIndex;
    int blockAbove;
    Color color1;
    Color color2;
    Color color3;
    switch (block2Type)
    {
        case 0:
            block1Type = grid[nextBlock].blockType;
            block2Type = grid[currentBlock].blockType;
            color1 = grid[nextBlock].color;
            color2 = grid[currentBlock].color;
            changeThird = false;
            break;
        case 11:
        case 8:
        case 4:
        case 2:
            blockAbove = nextBlock - gridSize.x;
            block2Type = grid[currentBlock].blockType;
            color2 = grid[currentBlock].color;
            if(grid[blockAbove].blockType == 0)
            {
                changeThird = true;
                block1Type = 0;
                color1 = Colors(0);
                block3Type = grid[nextBlock].blockType;
                color3 = grid[nextBlock].color;
                lastIndex = blockAbove;
            }
            else
            {
                block1Type = grid[nextBlock].blockType;
                color1 = grid[nextBlock].color;
                changeThird = false;
            }
            break;
        case 9:
            block2Type = 0;
            block1Type = 0;
            color1 = Colors(block1Type);
            color2 = Colors(block2Type);
            changeThird = false;
            break;
        default:
            block1Type = grid[currentBlock].blockType;
            block2Type = grid[nextBlock].blockType;
            color1 = grid[nextBlock].color;
            color2 = grid[currentBlock].color;
            changeThird = false;
    }

    blockReturner blockToReturn = {block1Type, currentBlock, color1, block2Type, nextBlock, color2, changeThird, block3Type, lastIndex, color3};
    return blockToReturn;
}


blockReturner GetBlockUnderForWater(block grid[], int currentBlock, int nextBlock)
{
    int block1Type = grid[currentBlock].blockType;
    int block2Type = grid[nextBlock].blockType;
    bool changeThird;
    int block3Type;
    int lastIndex;
    int blockAbove;
    Color color1;
    Color color2;
    Color color3;
    switch (block2Type)
    {
        case 0:
            block1Type = grid[nextBlock].blockType;
            block2Type = grid[currentBlock].blockType;
            color1 = grid[nextBlock].color;
            color2 = grid[currentBlock].color;
            changeThird = false;
            break;
        case 2:
            if (block1Type == 4)
            {
                block1Type = 5;
                block2Type = 5;
                color1 = Colors(5);
                color2 = Colors(5);
                changeThird = false;
            }
            break;
        case 4:
            if (block1Type == 2)
            {
                block1Type = 5;
                block2Type = 5;
                color1 = Colors(5);
                color2 = Colors(5);
                changeThird = false;
            }
            break;
        case 3:
        case 6:
            if(block1Type == 4)
            {
                block1Type = 4;
                block2Type = 7;
                color1 = Colors(block1Type);
                color2 = Colors(block2Type);
                changeThird = false;
            }
            break;
        case 11:
        case 8:
            if(block1Type != 8 && block1Type != 11)
            {
                block2Type = block1Type;
                block1Type = 8;
                color1 = Colors(block1Type);
                color2 = Colors(block2Type);
                changeThird = false;
            }
            break;
        case 9:
            block2Type = 0;
            block1Type = 0;
            color1 = Colors(block1Type);
            color2 = Colors(block2Type);
            changeThird = false;
            break;
        default:
            block1Type = grid[currentBlock].blockType;
            block2Type = grid[nextBlock].blockType;
            color1 = grid[currentBlock].color;
            color1 = grid[nextBlock].color;
            changeThird = false;
    }

    blockReturner blockToReturn = {block1Type, currentBlock, color1, block2Type, nextBlock, color2, changeThird, block3Type, lastIndex, color3};
    return blockToReturn;
}


blockReturner GetBlocksForFire(block grid[], int currentBlock, int nextBlock)
{
    int block1Type = grid[currentBlock].blockType;
    int block2Type = grid[nextBlock].blockType;
    bool changeThird;
    int block3Type;
    int lastIndex;
    int blockAbove;
    Color color1;
    Color color2;
    Color color3;
    switch (block2Type)
    {
        case 3:
        case 6:
            block1Type = grid[currentBlock].blockType;
            block2Type = grid[currentBlock].blockType;
            break;

        default:
            block1Type = grid[currentBlock].blockType;
            block2Type = grid[nextBlock].blockType;
            color1 = grid[currentBlock].color;
            color1 = grid[nextBlock].color;
            changeThird = false;
    }

    blockReturner blockToReturn = {block1Type, currentBlock, color1, block2Type, nextBlock, color2, changeThird, block3Type, lastIndex, color3};
    return blockToReturn;
}

blockReturner GetBlockUnderForAcid(block grid[], int currentBlock, int nextBlock)
{
    int block1Type = grid[currentBlock].blockType;
    int block2Type = grid[nextBlock].blockType;
    bool changeThird;
    int block3Type;
    int lastIndex;
    int blockAbove;
    Color color1;
    Color color2;
    Color color3;
    switch (block2Type)
    {
        case 0:
            block1Type = grid[nextBlock].blockType;
            block2Type = grid[currentBlock].blockType;
            color1 = grid[nextBlock].color;
            color2 = grid[currentBlock].color;
            changeThird = false;
            break;
        case 1:
        case 2:
        case 3:
        case 4:
        case 6:
            block1Type = 0;
            block2Type = 0;
            color1 = Colors(0);
            color2 = color1;
            changeThird = false;
            break;
        case 11:
        case 8:
            block2Type = block1Type;
            block1Type = grid[nextBlock].blockType;
            color1 = Colors(block1Type);
            color2 = Colors(block2Type);
            changeThird = false;
            break;        
        default:
            block1Type = grid[currentBlock].blockType;
            block2Type = grid[nextBlock].blockType;
            color1 = grid[currentBlock].color;
            color1 = grid[nextBlock].color;
            changeThird = false;
    }

    blockReturner blockToReturn = {block1Type, currentBlock, color1, block2Type, nextBlock, color2, changeThird, block3Type, lastIndex, color3};
    return blockToReturn;
}

bool checkBlocksForAcid(int i)
{
    switch (i)
    {
        case 0:
        case 1:
        case 2:
        case 3:
        case 4:
        case 6:
        case 8:
        case 11:
            return true;
        default:
            return false;
    }
}


blockReturner MaintainBlock(block grid[], int currentBlock, int nextBlock)
{
    int block1Type = grid[currentBlock].blockType;
    int block2Type = grid[nextBlock].blockType;
    Color color1 = grid[currentBlock].color;
    Color color2 = grid[nextBlock].color;
    blockReturner blockToReturn = {block1Type, currentBlock, color1, block2Type, nextBlock, color2};
    return blockToReturn;
}

bool BlockChecker(block grid[], int i)
{
    if(i > size)
        i = size;
    switch (grid[i].blockType)
    {
        case 0:
        case 2:
        case 4:
        case 8:
        case 9:
        case 11:
            return true;
            break;

        default:
            return false;
            break;
    }
}

Color Colors(int i)
{
    int random;
    switch (i)
    {
        case 0:
            return emptyColor;
        case 1:
            return (Color){255, 235 + GetRandomValue(-35, 5), 5 + GetRandomValue(-5, 35), 255};
        case 2:
            return (Color){0, 0 + GetRandomValue(100, 200), 255, 255};
        case 3:
            random = GetRandomValue(-20, 20);
            return (Color){100 + random, 25 + random/2, 0, 255};
        case 4:
            return (Color){255 + GetRandomValue(-25, 0), 150 + GetRandomValue(-75, 65), 25 + GetRandomValue(-25, 35), 255};
        case 5:
            random = GetRandomValue(-50, 50);
            return (Color){100 + random, 0, 100 + random, 255};
        case 6:
            random = GetRandomValue(0, 100);
            return (Color){0 + random, 0 + random, 0 + random, 255};
        case 7:
            return (Color){255 + GetRandomValue(-25, 0), 150 + GetRandomValue(-75, 25), 25 + GetRandomValue(-25, 35), 255};
        case 8:
            random = GetRandomValue(100, 230);
            return (Color){0 + random, 0 + random, 0 + random, 255};
        case 9:
            return (Color){0 + GetRandomValue(0, 60), 255 + GetRandomValue(-25, 0), 0 + GetRandomValue(0, 60), 255};
        case 10:
            return (Color){0 + GetRandomValue(0, 10), 0 + GetRandomValue(0, 10), 0 + GetRandomValue(0, 10), 255};
        case 11:
            return (Color){255 + GetRandomValue(-25, 0), 225 + GetRandomValue(-25, 10), 150 + GetRandomValue(-25, 25), 255};
        default:
            return (Color){255 + GetRandomValue(0, 3), 255 + GetRandomValue(0, 3), 255 + GetRandomValue(0, 3), 255};
    }
}

char *Name(int i)
{
    switch (i)
    {
        case 1:
            return "sand";
        case 2:
            return "water";
        case 3:
            return "wood";
        case 4:
            return "lava";
        case 5:
            return "obsidian";
        case 6:
            return "coal";
        case 7:
            return "fire";
        case 8:
            return "smoke";
        case 9:
            return "acid";
        case 10:
            return "explosive";
        default:
            return "weird";
    }
}

void RestartAllBlocks(block grid[])
{
    for(int i = 0; i<size; i++)
    {
        grid[i] = ReturnBlock(grid[i].pos, 0);
    }
}