#pragma once
struct Position {
	int x;
	int y;
	bool operator ==(const Position& other) const {//Positionlar eþit mi deðil mi bakmak için == operatorünü güncelledik
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
namespace std {//position için hash fonksiyonu yazýyoruz
	template<> struct hash<Position> {
		size_t operator()(const Position& p)const {
			return hash<int>()(p.x) ^ (hash<int>()(p.y) << 1);//deðerleri ayrý ayrý hashleyip XOR ile birleþtiriyoruz (<<1 hash çeþitliliði saðlayýp çakýþmalarý engellemek için)
		}
	};
}