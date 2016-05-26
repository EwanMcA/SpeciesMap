
#include "map.h"
#include <time.h>

#include <iostream>

Map::Map(int x_vertices, int z_vertices, int noise)
{
	srand(time(NULL));
	for (int k = 0; k < z_vertices; k++) {
		std::vector<Vertex> row;
		vertices.push_back(row);
		for (int i = 0; i < x_vertices; i++) {
			//Vertex v(i, k, (std::rand() % noise) / 5, (std::rand() % noise) / 10, (std::rand() % noise) / 5);
			Vertex v(i, k, 0.0f, 0.0f, 0.0f);
			vertices[k].push_back(v);
		}
	}
}

void Map::clear(int x_vertices, int z_vertices)
{
	vertices.clear();
	for (int k = 0; k < z_vertices; k++) {
		std::vector<Vertex> row;
		vertices.push_back(row);
		for (int i = 0; i < x_vertices; i++) {
			Vertex v(i, k, 0.0f, 0.0f, 0.0f);
			vertices[k].push_back(v);
		}
	}
}

// diamond - square algorithm.
void Map::randomise(float range, float square_frac)
{
	int square_side = (int)(vertices.size() - 1)*square_frac;
	float avg = 0.0f;
	float sum = 0.0f;
	// Diamond Step
	for (int z = 0; z + square_side < vertices.size(); z += square_side) {
		for (int x = 0; x + square_side < vertices[0].size(); x += square_side) {
			avg = (vertices[z][x].getElev() + vertices[z + square_side][x].getElev() +
				vertices[z][x + square_side].getElev() + vertices[z + square_side][x + square_side].getElev())*0.25f;
			vertices[z+square_side*0.5f][x+square_side*0.5f].setElev(avg + (((float)std::rand() / (float)RAND_MAX)*range*2) - range);
		}
	}
	// Square Step
	for (int z = 0; z + square_side < vertices.size(); z += square_side) {
		for (int x = 0; x + square_side < vertices[0].size(); x += square_side) {
			sum = vertices[z][x].getElev() + vertices[square_side*0.5f][x + square_side*0.5f].getElev() +
				vertices[z][x + square_side].getElev();
			if (z != 0) {
				sum += vertices[z - square_side*0.5f][x + square_side*0.5f].getElev();
				avg = sum / 4;
			} 
			else { avg = sum / 3; }
			vertices[z][x + square_side*0.5f].setElev(avg + (((float)std::rand() / (float)RAND_MAX)*range * 2) - range);
			sum = vertices[z][x].getElev() + vertices[z + square_side*0.5f][square_side*0.5f].getElev() +
				vertices[z + square_side][x].getElev();
			if (x != 0) {
				sum += vertices[z + square_side*0.5f][x - square_side*0.5f].getElev();
				avg = sum / 4;
			}
			else { avg = sum / 3; }
			vertices[z + square_side*0.5f][x].setElev(avg + (((float)std::rand() / (float)RAND_MAX)*range * 2) - range);
			sum = vertices[z][x+square_side].getElev() + vertices[z + square_side*0.5f][x + square_side*0.5f].getElev() +
				vertices[z + square_side][x + square_side].getElev();
			if (x + square_side != vertices[0].size() - 1) {
				sum += vertices[z + square_side*0.5f][x + square_side*1.5f].getElev();
				avg = sum / 4;
			}
			else { avg = sum / 3; }
			vertices[z + square_side*0.5f][x + square_side].setElev(avg + (((float)std::rand() / (float)RAND_MAX)*range * 2) - range);
			sum = vertices[z + square_side][x].getElev() + vertices[z + square_side*0.5f][x + square_side*0.5f].getElev() +
				vertices[z + square_side][x + square_side].getElev();
			if (z + square_side != vertices.size() - 1) {
				sum += vertices[z + square_side*1.5f][x + square_side*0.5f].getElev();
				avg = sum / 4;
			}
			else { avg = sum / 3; }
			vertices[z + square_side][x + square_side*0.5f].setElev(avg + (((float)std::rand() / (float)RAND_MAX)*range * 2) - range);
		}
	}
	if (square_side == 1) { return; }
	else { randomise(range*0.5f, square_frac*0.5f); }
}

void Map::smooth(int dist) {

	for (int k = 0; k < vertices.size(); k++) {
		for (int i = 0; i < vertices[0].size(); i++) {
			float sum = 0.0f;
			int count = 0;
			for (int s = -dist; s < 1 + dist; s++) {
				for (int r = -dist; r < 1 + dist; r++) {
					if (s == 0 && r == 0) { continue; }
					if (k + s < 0 || i + r < 0 || k + s > vertices.size() - 1 || i + r > vertices[0].size()-1) { continue; }
					sum += vertices[k + s][i + r].getElev();
					count += 1;
				}
			}
			vertices[k][i].setElev(sum / (float)count);
		}
	}

}

Vertex::Vertex(int i, int k, float elev, float temp, float prec)
:i(i), k(k), elev(elev), temp(temp), prec(prec)
{

}