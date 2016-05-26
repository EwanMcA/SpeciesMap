#pragma once

#include <vector>
#include <iostream>

class Vertex {
	int i;
	int k;
	float elev;
	float temp;
	float prec;
	bool spec = false;
public:
	Vertex(int i, int k, float height, float temp, float prec);  // Will I need i,k?
	float getElev() const { return elev; }
	float getTemp() const { return temp; }
	float getPrec() const { return prec; }
	void setElev(float height) { elev = height; }
	void addElev(float amount) { elev += amount; }
	void addTemp(float amount) { temp += amount; }
	void addPrec(float amount) { prec += amount; }
};

class Map {
	std::vector<std::vector<Vertex>> vertices;
public:
	Map(int x_vertices, int z_vertices, int noise);
	void clear(int x_vertices, int z_vertices);
	float getElev(int i, int k) { return vertices[k][i].getElev(); }
	void addElev(int i, int k, float amount) { vertices[k][i].addElev(amount); }
	float getTemp(int i, int k) { return vertices[k][i].getTemp(); }
	void addTemp(int i, int k, float amount) { vertices[k][i].addTemp(amount); }
	float getPrec(int i, int k) { return vertices[k][i].getPrec(); }
	void addPrec(int i, int k, float amount) { vertices[k][i].addPrec(amount); }
	void randomise(float range, float square_frac);
	void smooth(int dist);
};