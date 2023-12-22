#pragma once
#include "Engine/config.h"
#include "Engine/Global illumination/PackedLookupTable/PackedLookupTable.h"

template<class T>
class LogarithmicBin {
private:

#pragma mark - Nested types

	using Index = uint64_t;

	struct BinObject {
		T object;
		float weight = 0.0f;

		BinObject(const T& object, float weight) : object(object), weight(weight) {
		}
	};

	struct Bin {
		PackedLookupTable<BinObject> objects;
		float totalWeight = 0.0f;

		Bin() : objects(1000) {
		}
	};

	using BinsIterator = typename std::unordered_map<Index, Bin>::iterator;
	using BinObjectsIterator = typename PackedLookupTable<BinObject>::Iterator;

#pragma mark Forward iterator

public:
	class Iterator {
	private:
		friend LogarithmicBin;

		BinsIterator mBinsIterator;
		BinsIterator mBinsEndIterator;
		BinObjectsIterator mBinObjectsIterator;

		Iterator(BinsIterator i, BinsIterator endIterator, BinObjectsIterator binObjectsIterator);

		Iterator(BinsIterator endIterator);

	public:
		Iterator& operator++();

		T& operator*();

		T* operator->();

		const T& operator*() const;

		const T* operator->() const;

		bool operator!=(const Iterator& other) const;
	};

#pragma mark - Private members

private:
	std::mt19937 mEngine;
	std::uniform_real_distribution<float> mDistribution;
	std::unordered_map<Index, Bin> mBins;

	float mMinWeight = 0.0f;
	float mMaxWeight = 0.0f;
	float mTotalWeight = 0.0f;
	uint64_t mSize = 0;

	int32_t index(float weight);

	float binMinWeight(Index index);

	float binMaxWeight(Index index);

#pragma mark - Public members

public:
	LogarithmicBin(float minWeight, float maxWeight);

	LogarithmicBin(float maxWeight);

	float minWeight() const;

	float maxWeight() const;

	float totalWeight() const;

	uint64_t size() const;

	bool empty() const;

	void insert(const T& object, float weight);

	void erase(const Iterator& it);

	Iterator random();

#pragma mark Iteration

	Iterator begin();

	Iterator end();
};

template<typename T>
typename LogarithmicBin<T>::Iterator begin(const LogarithmicBin<T>& bin) {
	return bin.begin();
}

template<typename T>
typename LogarithmicBin<T>::Iterator end(const LogarithmicBin<T>& bin) {
	return bin.end();
}



#include "LogarithmicBinImpl.h"
#include "LogarithmicBinIteratorImpl.h"