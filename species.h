#pragma once

class Species {
	float alt_range[2];
	float temp_range[2];
	float prec_range[2];
public:
	Species(float alt_range[2], float temp_range[2], float prec_range[2]);
	float getAltLower() const { return alt_range[0]; }
	float getAltUpper() const { return alt_range[1]; }
	float getTempLower() const { return temp_range[0]; }
	float getTempUpper() const { return temp_range[1]; }
	float getPrecLower() const { return prec_range[0]; }
	float getPrecUpper() const { return prec_range[1]; }
	void setAltLower(float limit) { alt_range[0] = limit; }
	void setAltUpper(float limit) { alt_range[1] = limit; }
	void setTempLower(float limit) { temp_range[0] = limit; }
	void setTempUpper(float limit) { temp_range[1] = limit; }
	void setPrecLower(float limit) { prec_range[0] = limit; }
	void setPrecUpper(float limit) { prec_range[1] = limit; }
};