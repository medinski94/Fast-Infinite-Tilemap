#ifndef PAGE_GENERATOR_H
#define PAGE_GENERATOR_H

#include <SFML/Graphics.hpp>
#include <FastNoiseSIMD.h>

struct NoiseOptions {
	int seed;
	float frequency;
	int octaves;
	unsigned int numTypes;
	unsigned int numVariations;
};

class PageGenerator {
	NoiseOptions options;
public:
	PageGenerator(const NoiseOptions& options) {
		this->options = options;
	}

	const sf::Uint8* operator()(const sf::IntRect& bounds) {
		FastNoiseSIMD* noise = FastNoiseSIMD::NewFastNoiseSIMD();

		noise->SetNoiseType(FastNoiseSIMD::NoiseType::PerlinFractal);
		noise->SetSeed(options.seed);
		noise->SetFractalOctaves(5);

		float* terrainData = noise->GetNoiseSet(
			0,
			bounds.top,
			bounds.left,
			1,
			bounds.width,
			bounds.height
		);

		noise->SetFrequency(0.002f);
		float* waterData = noise->GetNoiseSet(
			0,
			bounds.top,
			bounds.left,
			1,
			bounds.width,
			bounds.height
		);

		sf::Uint8* data = new sf::Uint8[bounds.width * bounds.height];

		for (int i = 0; i < bounds.width * bounds.height; i++) {
			int value = std::clamp((terrainData[i] + waterData[i] * 5.0f) / 2.0f + 0.5f, 0.0f, 1.0f) * (this->options.numTypes - 0.1f);
			value *= this->options.numVariations;
			value += std::rand() % this->options.numVariations;
			data[i] = value;
		}

		FastNoiseSIMD::FreeNoiseSet(terrainData);
		FastNoiseSIMD::FreeNoiseSet(waterData);

		return data;
	}
};

#endif