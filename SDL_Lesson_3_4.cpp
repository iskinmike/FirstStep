#include <SDL.h>
#include <SDL_image.h>


#include <iostream>
#include <string>



// Блок описания констант и прочего. Глобальных
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;
//We'll just be using square tiles for now
const int TILE_SIZE = 40;

//Глобальными объявлениями окна и рендерера, чтобы они были доступны из наших функций. Так же мы инициализируем указатели как nullptr для безопасности. 
//Если вы не используете C++11, инициализируйте их как NULL
SDL_Window *window = nullptr; // 
SDL_Renderer *renderer = nullptr;


// создаем функцию которая будет загружать наши текстуры по имени файла
SDL_Texture* LoadImage(std::string file){
	SDL_Surface *loadedImage = nullptr; // инициализация указателей, через нулевой на поверхность и структуру
	SDL_Texture *texture = nullptr;
	loadedImage = SDL_LoadBMP(file.c_str()); // загрузка БМП изображения
	if (loadedImage != nullptr){
		texture = SDL_CreateTextureFromSurface(renderer, loadedImage); // создает текстуру из нашей поверхности
		SDL_FreeSurface(loadedImage); // освобождает память. Удаляет из нее загруженный файл БМП
	}
	else
		std::cout << SDL_GetError() << std::endl;
	return texture;
}


/**
* Loads an image into a texture on the rendering device
* @param file The image file to load
* @param ren The renderer to load the texture onto
* @return the loaded texture, or nullptr if something went wrong.
*/
SDL_Texture* loadTexture(const std::string &file, SDL_Renderer *ren){
	SDL_Texture *texture = IMG_LoadTexture(ren, file.c_str());
	if (texture == nullptr){
		//logSDLError(std::cout, "LoadTexture");
		std::cout << SDL_GetError() << std::endl;
	}
	return texture;
}

/**
* Draw an SDL_Texture to an SDL_Renderer at position x, y, with some desired
* width and height
* @param tex The source texture we want to draw
* @param ren The renderer we want to draw to
* @param x The x coordinate to draw to
* @param y The y coordinate to draw to
* @param w The width of the texture to draw
* @param h The height of the texture to draw
*/
void renderTexture(SDL_Texture *tex, SDL_Renderer *ren, int x, int y, int w, int h){
	//Setup the destination rectangle to be at the position we want
	SDL_Rect dst;
	dst.x = x;
	dst.y = y;
	dst.w = w;
	dst.h = h;
	SDL_RenderCopy(ren, tex, NULL, &dst);
}

/**
* Draw an SDL_Texture to an SDL_Renderer at position x, y, preserving
* the texture's width and height
* @param tex The source texture we want to draw
* @param ren The renderer we want to draw to
* @param x The x coordinate to draw to
* @param y The y coordinate to draw to
*/
void renderTexture(SDL_Texture *tex, SDL_Renderer *ren, int x, int y){
	int w, h;
	SDL_QueryTexture(tex, NULL, NULL, &w, &h);
	renderTexture(tex, ren, x, y, w, h);
}


// создаем функцию для рисования текстуры. Она принимает место(х,у), указатель на текстуру и указатель на рендерер(визуализатор)
void ApplySurface(int x, int y, SDL_Texture *tex, SDL_Renderer *rend){
	SDL_Rect pos; // Это стандартный прямоугольник. 4 свойства. х,у - положение угла видимо и w,h - ширина и высота прямоугольника. видимо в пикселах
	pos.x = x;
	pos.y = y;
	SDL_QueryTexture(tex, NULL, NULL, &pos.w, &pos.h); //какие-то функции для отрисовки. Заполняет w и h информацией о высоте и ширине структуры
	SDL_RenderCopy(rend, tex, NULL, &pos);             //Оставшийся NULL параметр, переданный этой функции отвечает за вырезание фрагмента исходной текстуры
}






int main(int argc, char *argv[]){// обязательно аргументы, без них не работает


	if (SDL_Init(SDL_INIT_EVERYTHING) == -1){
		std::cout << SDL_GetError() << std::endl;
		return 1;
	}

	if ((IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG) != IMG_INIT_PNG){
		//logSDLError(std::cout, "IMG_Init");
		std::cout << SDL_GetError() << std::endl;
		SDL_Quit();
		return 1;
	}

	// Создаем окно
	window = SDL_CreateWindow("Lesson 2", SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN); 

	//вернем ошибку если не получилось
	if (window == nullptr){
		std::cout << SDL_GetError() << std::endl;
		return 2;
	}

	// запустим инициализированный нами рендерер
	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED| SDL_RENDERER_PRESENTVSYNC); // надо бы про параметры описать

	//вернем ошибку если не получилось
	if (renderer == nullptr){
		std::cout << SDL_GetError() << std::endl;
		return 3;
	}

	// Загрузим с помошью ранее описанных функций 
	SDL_Texture *background = loadTexture("./res/background.png", renderer);
	SDL_Texture *image = loadTexture("./res/image.png", renderer);
	//Make sure they both loaded ok
	if (background == nullptr || image == nullptr){
		//cleanup(background, image, renderer, window);
		IMG_Quit();
		SDL_Quit();
		return 4;
	}


	// теерь начнем рисовать 
	// сначала отчистим рендерер
	SDL_RenderClear(renderer);


	//Determine how many tiles we'll need to fill the screen
	int xTiles = SCREEN_WIDTH / TILE_SIZE;
	int yTiles = SCREEN_HEIGHT / TILE_SIZE;

	//Draw the tiles by calculating their positions
	for (int i = 0; i < xTiles * yTiles; ++i){
		int x = i % xTiles;
		int y = i / xTiles;
		renderTexture(background, renderer, x * TILE_SIZE, y * TILE_SIZE, TILE_SIZE,
			TILE_SIZE);
	}


	int iW, iH;
	SDL_QueryTexture(image, NULL, NULL, &iW, &iH);
	int x = SCREEN_WIDTH / 2 - iW / 2;
	int y = SCREEN_HEIGHT / 2 - iH / 2;
	renderTexture(image, renderer, x, y);

	// покажем рендерер и подождем чтобы увидеть результат. Получается в памяти эти изображения наложены друг на друга - отрендерены и надо просто показать их.
	SDL_RenderPresent(renderer);

	//Our event structure
	SDL_Event e;
	bool quit = false;
	while (!quit){
		while (SDL_PollEvent(&e)){
			if (e.type == SDL_QUIT){
				quit = true;
			}
			if (e.type == SDL_KEYDOWN){
				quit = true;
			}
			if (e.type == SDL_MOUSEBUTTONDOWN){
				quit = true;
			}
		}
		//Render the scene
		SDL_RenderClear(renderer);
		renderTexture(image, renderer, x, y);
		SDL_RenderPresent(renderer);
	}

	return 0;
}
