#include <emscripten.h>
#include <iostream>
#include <cmath>
#include <stdlib.h>

using namespace std;

struct particle
{
    double x = 0;
    double y = 0;
    double vx = 0;
    double vy = 0;
};

const int ParticlesCount = 25000;
const int GridSide = 23;

typedef particle pile[ParticlesCount];
typedef int sectorData[ParticlesCount]; // / (GridSide * GridSide) * 5];

struct sector
{
    sectorData habitatants;
    int count = 0;
    double dragX = 0;
    double dragY = 0;
};




const double Viscosity = 2;
const double brownian = 0.05;

typedef sector sectorRow[GridSide];
typedef sectorRow grid[GridSide];

const int SectorSide = 10;

const int particlesPerSector = ParticlesCount / (GridSide * GridSide);
const int practicalParticlesPerSector = ParticlesCount / (GridSide - 2 * GridSide - 2);
const int minParticlesPerSector = particlesPerSector * 0.2;

int particleCount = 0;

struct Data
{
    pile _pile;
    grid _grid;
};

particle MouseForce;

void reorganize(grid &grid, pile &pile)
{
    //cout << "reorganize" << endl;
    for (int y = 0; y < GridSide; y++)
    {
        for (int x = 0; x < GridSide; x++)
        {
            // while (false && minParticlesPerSector > grid[y][x].count)
            // {
            //     //cout << "to few particles inside, presure: " << grid[y][x].count << "being max " << particlesPerSector << endl;
            //     int VictimX = rand() % GridSide;
            //     int VictimY = rand() % GridSide;
            //     //cout << "maybe taking particle from" << VictimX << "," << VictimY << endl;
            //     if (grid[VictimY][VictimX].count > minParticlesPerSector)
            //     {

            //         grid[VictimY][VictimX].count--;
            //         int lastPos = grid[y][x].count;
            //         grid[y][x].habitatants[lastPos] = grid[VictimY][VictimX].habitatants[grid[VictimY][VictimX].count];
            //         pile[grid[y][x].habitatants[lastPos]].x = x * SectorSide + (rand() % (SectorSide * 1000)) / 1000.;
            //         pile[grid[y][x].habitatants[lastPos]].y = y * SectorSide + (rand() % (SectorSide * 1000)) / 1000.;
            //         pile[grid[y][x].habitatants[lastPos]].vx = grid[y][x].dragX;
            //         pile[grid[y][x].habitatants[lastPos]].vy = grid[y][x].dragY;
            //         grid[y][x].count++;
            //         //cout << "particle taken" << endl;
            //     }
            // }

            //cout << "antes del for" << endl;

            for (int h = 0; h < grid[y][x].count; h++)
            {
                //cout << "principio del for" << endl;
                int particleI = grid[y][x].habitatants[h];
                if (particleI > particleCount)
                {
                    cout << "me rompo" << endl;
                }
                int newY = floor(pile[particleI].y / SectorSide);
                int newX = floor(pile[particleI].x / SectorSide);
                //cout << "antes del if" << endl;

                if (newY != y || newX != x)
                {
                    //cout << "the particle " << particleI << " of " << particleCount << " left the sector x:" << x << " , y:" << y << " being its cordinates: x: "<< pile[particleI].x << ", y:" << pile[particleI].y <<", resituating" << endl;

                    //cout << "acessing sector x:" << newX << ", y: " << newY << endl;

                    grid[newY][newX].habitatants[grid[newY][newX].count] = particleI;
                    grid[newY][newX].count++;

                    //cout << "current list state of new sector:";
                    //for (int j = 0; j < grid[newY][newX].count; j++)
                    //{
                    //    cout << grid[newY][newX].habitatants[j] << ", ";
                    //}
                    //cout << endl;

                    //cout << "removing from original sector x:" << x << ", y: " << y << endl;

                    grid[y][x].count--;
                    grid[y][x].habitatants[h] = grid[y][x].habitatants[grid[y][x].count];
                    h--;
                    //cout << "current list state of sector:";
                    //for (int j = 0; j < grid[y][x].count; j++)
                    //{
                    //cout << grid[y][x].habitatants[j] << ", ";
                    //}
                    //cout << endl;

                    //cout << "susces!!" << endl;
                }
                //cout << "endl del for" << endl;
            }

            //cout << "fake intersector presure" << endl;

            if (y != 0)
            {
                grid[y][x].dragY -= min((grid[y][x].count - grid[y - 1][x].count) * 0.005, 0.1);
            }

            if (y != GridSide - 1)
            {
                grid[y][x].dragY += min((grid[y][x].count - grid[y + 1][x].count) * 0.005, 0.1);
            }

            if (x != 0)
            {
                grid[y][x].dragX -= min((grid[y][x].count - grid[y][x - 1].count) * 0.005, 0.1);
            }

            if (x != GridSide - 1)
            {
                grid[y][x].dragX += min((grid[y][x].count - grid[y][x + 1].count) * 0.005, 0.1);
            }

            if (isnan(grid[y][x].dragX) || isnan(grid[y][x].dragY))
            {
                cout << "presure sim" << endl;
            }
            //cout << "sucscess!!(fake presure)" << endl;

            //cout << "redoing drag, population of sector: " << grid[y][x].count << endl;
            double oldDragX = grid[y][x].dragX;
            double oldDragY = grid[y][x].dragY;

            grid[y][x].dragX = 0;
            grid[y][x].dragY = 0;
            if (grid[y][x].count != 0)
            {

                for (int h = 0; h < grid[y][x].count; h++)
                {
                    int particleI = grid[y][x].habitatants[h];
                    if (isnan(pile[particleI].vx) || isnan(pile[particleI].vx))
                    {
                        cout << "las velocidades" << endl;
                    }
                    grid[y][x].dragX += pile[particleI].vx;
                    grid[y][x].dragY += pile[particleI].vy;
                }
                if (isnan(grid[y][x].dragY) || isnan(grid[y][x].dragX))
                {
                    cout << "el drag 0" << endl;
                }
                grid[y][x].dragX /= grid[y][x].count;
                grid[y][x].dragY /= grid[y][x].count;

                if (isnan(grid[y][x].dragY) || isnan(grid[y][x].dragX))
                {
                    cout << "el drag 0.4 :" << grid[y][x].count << endl;
                }

                grid[y][x].dragX += oldDragX;
                grid[y][x].dragY += oldDragY;
                grid[y][x].dragX /= 2;
                grid[y][x].dragY /= 2;
                if (isnan(grid[y][x].dragY) || isnan(grid[y][x].dragX))
                {
                    cout << "el drag 1" << endl;
                }
            }

            //cout << "drag done" << endl;

            //cout << "drag of x:" << x << ", y:" << y << ", dragX: " << grid[y][x].dragX << ", dragY:" << grid[y][x].dragY << endl;
        }
    }
    if (true)
    {
        //cout << "force field" << endl;

        //grid[7][5].dragY = 10;
        // grid[7][4].dragY = 10;
        // grid[7][3].dragY = 10;
        // grid[7][2].dragY = 10;
        // grid[7][1].dragY = 10;
        // grid[7][0].dragY = 10;
        //  grid[7][9].dragY = 10;
        //  grid[7][10].dragY = 10;
        //  grid[7][11].dragY = 10;
        //  grid[7][12].dragY = 10;
        //  grid[7][13].dragY = 10;
        //  grid[7][14].dragY = 10;

        int sectorX = MouseForce.x / SectorSide;
        int sectorY = MouseForce.y / SectorSide;
        if (sectorX > 0 && sectorX < GridSide && sectorY > 0 && sectorY < GridSide && !isnan(MouseForce.vx) && !isnan(MouseForce.vy))
        {
            grid[sectorY][sectorX].dragX = MouseForce.vx;
            grid[sectorY][sectorX].dragY = MouseForce.vy;
        }

        for (int i = 0; i < GridSide; i++)
        {
            grid[0][i].dragY = +.1;
            grid[GridSide - 1][i].dragY = -.1;
            grid[i][0].dragX = +.1;
            grid[i][GridSide - 1].dragX = -.1;
        }

        //cout << "force field success" << endl;
    }
    //cout << "reorganitation done" << endl;
}

double min(double a, double b)
{
    if (a > b)
    {
        return b;
    }
    else
        return a;
}

double sectorDragInfluence(grid &grid, pile &pile, int PilePos, int sectorX, int sectorY)
{
    int x = pile[PilePos].x;
    int y = pile[PilePos].y;

    double distance = max(abs(sectorX * SectorSide + (SectorSide / 2) - x), abs(sectorY * SectorSide + (SectorSide / 2) - y)) / SectorSide;
    double ret = min(.1, max(.0, -distance + 1));
    //cout << "influence: " << ret << endl;
    return ret;
}

void simulateStep(grid &grid, pile &pile, double tDelta)
{
    //cout << "step" << endl;
    reorganize(grid, pile);
    //cout << "proceding with sim" << endl;
    for (int y = 0; y < GridSide; y++)
    {
        for (int x = 0; x < GridSide; x++)
        {
            for (int h = 0; h < grid[y][x].count; h++)
            {
                int particleI = grid[y][x].habitatants[h];

                //cout << "sector: x:" << x << " y:" << y << " particle:" << particleI << endl;

                pile[particleI].x += pile[particleI].vx * tDelta;
                pile[particleI].y += pile[particleI].vy * tDelta;

                double internalPresure = 0.04;

                pile[particleI].vx += ((rand() % 2000 / 1000.) - 1) * brownian * (GridSide * SectorSide) / 100 + internalPresure * ((x + .5) * SectorSide - pile[particleI].x) * -(grid[y][x].count - practicalParticlesPerSector) * ((rand() % 1000) / 1000) / practicalParticlesPerSector;
                pile[particleI].vy += ((rand() % 2000 / 1000.) - 1) * brownian * (GridSide * SectorSide) / 100 + internalPresure * ((y + .5) * SectorSide - pile[particleI].y) * -(grid[y][x].count - practicalParticlesPerSector) * ((rand() % 1000) / 1000) / practicalParticlesPerSector;

                double margin = 0.6;
                double sideDamping = 0.2;

                if (pile[particleI].x < margin * SectorSide)
                {
                    pile[particleI].x = margin * SectorSide;
                    pile[particleI].vx *= -sideDamping;
                }
                if (pile[particleI].y < margin * SectorSide)
                {
                    pile[particleI].y = margin * SectorSide;
                    pile[particleI].vy *= -sideDamping;
                }

                double maxPos = (GridSide - margin) * SectorSide;

                if (pile[particleI].x > maxPos)
                {
                    pile[particleI].x = maxPos;
                    pile[particleI].vx *= -sideDamping;
                }
                if (pile[particleI].y > maxPos)
                {
                    pile[particleI].y = maxPos;
                    pile[particleI].vy *= -sideDamping;
                }

                //cout << " drag calculations" << endl;

                const int deltaX[5] = {0, 1, -1, 0, 0};
                const int deltaY[5] = {0, 0, 0, 1, -1};
                double TotalInfluence = 0;
                int deltaCount = 5;
                for (int d = 0; d < deltaCount; d++)
                {
                    if (y + deltaY[d] >= 0 && y + deltaY[d] < GridSide && x + deltaX[d] >= 0 && x + deltaX[d] < GridSide)
                    {
                        double influence = sectorDragInfluence(grid, pile, particleI, x + deltaX[d], y + deltaY[d]);
                        TotalInfluence += influence;
                        pile[particleI].vx += grid[y + deltaY[d]][x + deltaX[d]].dragX * Viscosity * influence;
                        pile[particleI].vy += grid[y + deltaY[d]][x + deltaX[d]].dragY * Viscosity * influence;
                        if (isnan(influence))
                        {
                            cout << "influence" << endl;
                        }
                        if (isnan(pile[particleI].vx) || isnan(pile[particleI].vy))
                        {
                            cout << "las velocidades 2" << endl;
                        }
                        if (isnan(grid[y + deltaY[d]][x + deltaX[d]].dragY) || isnan(grid[y + deltaY[d]][x + deltaX[d]].dragX))
                        {
                            cout << "el drag 2" << endl;
                        }
                    }
                }
                pile[particleI].vx /= 1 + Viscosity * TotalInfluence;
                pile[particleI].vy /= 1 + Viscosity * TotalInfluence;
            }
        }
    }
}
bool ready = false;
Data* data;

void EMSCRIPTEN_KEEPALIVE JSStep(double tDelta)
{
    if (ready)
        simulateStep(data->_grid, data->_pile, tDelta);
}

int main()
{
    data = new Data();
    for (int y = 0; y < GridSide; y++)
    {
        for (int x = 0; x < GridSide; x++)
        {
            for (int h = 0; h < particlesPerSector; h++)
            {
                data->_pile[particleCount].x = x * SectorSide + ((rand() % 100000) / 100000.) * SectorSide;
                data->_pile[particleCount].y = y * SectorSide + ((rand() % 100000) / 100000.) * SectorSide;
                data->_grid[y][x].habitatants[data->_grid[y][x].count] = particleCount;
                data->_grid[y][x].count++;
                particleCount++;
            }

        }
    }
    ready = true;
    return 0;
}

//EMSCRIPTEN_KEEPALIVE

void EMSCRIPTEN_KEEPALIVE JSRefreshMouseVector(double x, double y, double vx, double vy)
{
    //cout << "x:" << x << " y:" << y << " vx:" << vx << " vy:" << vy << endl;
    MouseForce.x = x * SectorSide * GridSide;
    MouseForce.y = y * SectorSide * GridSide;
    MouseForce.vx = vx * SectorSide * GridSide;
    MouseForce.vy = vy * SectorSide * GridSide;
}

int EMSCRIPTEN_KEEPALIVE JSGetParticleCount()
{
    return particleCount;
}

double EMSCRIPTEN_KEEPALIVE JSGetX(int i)
{
    return data->_pile[i].x;
}

double EMSCRIPTEN_KEEPALIVE JSGetY(int i)
{
    return data->_pile[i].y;
}

double EMSCRIPTEN_KEEPALIVE JSGetSide(int i)
{
    return GridSide * SectorSide;
}