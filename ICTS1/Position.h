#pragma once
struct Position {
	int x;
	int y;
	bool operator ==(const Position& other) const {//Positionlar e�it mi de�il mi bakmak i�in == operator�n� g�ncelledik
		return x == other.x &&
			y == other.y;
	}
	bool operator !=(const Position& other) const {
		return !(*this == other);

	}
	bool operator <(const Position& other) const {
		return(x < other.x) || (x == other.x && y < other.y);
	}
};
#include <functional>
namespace std {//position i�in hash fonksiyonu yaz�yoruz
	template<> struct hash<Position> {
		size_t operator()(const Position& p)const {
			return hash<int>()(p.x) ^ (hash<int>()(p.y) << 1);//de�erleri ayr� ayr� hashleyip XOR ile birle�tiriyoruz (<<1 hash �e�itlili�i sa�lay�p �ak��malar� engellemek i�in)
		}
	};
}