#include <SDL.h>

#include <iostream>
#include <string>



// Блок описания констант и прочего. Глобальных
const int SCREEN_WIDTH = 640; // соответственно ширина и высота экрана
const int SCREEN_HEIGHT = 480;


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
	SDL_Texture *background = nullptr, *image = nullptr; //инициализировали 2 указателя на текстуры
	background = LoadImage("./res/background.bmp"); // функции возвращают указатель на уже созданную структуру
	image = LoadImage("./res/image.bmp");
	// выведем ошибку если не сработало
	if (background == nullptr || image == nullptr)
		return 4;

	// теерь начнем рисовать 
	// сначала отчистим рендерер
	SDL_RenderClear(renderer);

	int bW, bH; // в жэти переменные передадим значения ширины и высоты изображения бэкграунда
	SDL_QueryTexture(background, NULL, NULL, &bW, &bH); // с помошью этой функции в переменные запишем ширину и высоту
	//рисуем, описанной ранее функцией
	ApplySurface(0, 0, background, renderer);
	ApplySurface(bW, 0, background, renderer);
	ApplySurface(0, bH, background, renderer);
	ApplySurface(bW, bH, background, renderer);

	// Теперь рисуем изображение по центру экрана
	int iW, iH; // опять переменные ширины и высоты
	SDL_QueryTexture(image, NULL, NULL, &iW, &iH); // тперь в них записываем значения
	int x = SCREEN_WIDTH / 2 - iW / 2; // считаем
	int y = SCREEN_HEIGHT / 2 - iH / 2;
	ApplySurface(x, y, image, renderer); // рисуем

	// покажем рендерер и подождем чтобы увидеть результат. Получается в памяти эти изображения наложены друг на друга - отрендерены и надо просто показать их.
	SDL_RenderPresent(renderer);
	SDL_Delay(2000);

	// все чистим и завершаем
	SDL_DestroyTexture(background);
	SDL_DestroyTexture(image);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();

	/*
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0){
		std::cout << "SDL_Init Error: " << SDL_GetError() << std::endl;
		return 1;
	}
	
	// создаем окно. Указатель типа SDL_Window, получает видимо ссылку на объект окна со свойствами заданными конструктором.
	SDL_Window *win = SDL_CreateWindow("Hello World!", 100, 100, 640, 480, SDL_WINDOW_SHOWN);
	if (win == nullptr){
		std::cout << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
		return 1;
	}

	// Создаем Рендерер. он позволит рисовать в созданном окне. Он принимает указатель на объект окна
	SDL_Renderer *ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (ren == nullptr){
		std::cout << "SDL_CreateRenderer Error: " << SDL_GetError() << std::endl;
		return 1;
	}

	// Записываем в память бмп рисунок вункцией LoadBmp, И запоминаем на него указатель типа Surface
	SDL_Surface *bmp = SDL_LoadBMP("./res/Hello.bmp");
	if (bmp == nullptr){
		std::cout << "SDL_LoadBMP Error: " << SDL_GetError() << std::endl;
		return 1;
	}

	// Загрузим изображение в рендерер через CreateTextureFromSurface. Мы передаем контекст рендеринга и картинку в памяти (SDL_Surface), а получаем загруженную текстуру.
	SDL_Texture *tex = SDL_CreateTextureFromSurface(ren, bmp);
	SDL_FreeSurface(bmp); // освобождает память( очищает/удаляет переменную bmp)
	if (tex == nullptr){
		std::cout << "SDL_CreateTextureFromSurface Error: " << SDL_GetError() << std::endl;
		return 1;
	}

	//
	SDL_RenderClear(ren); // очищаем наш рендерер(переменная ren)
	SDL_RenderCopy(ren, tex, NULL, NULL); // копируем нашу текстуру в ren. т.е. теперь в рендерится наша текстура. чет не совсем понимаю
	SDL_RenderPresent(ren); // выводим то что у нас в рендере. видимо это называется рендерить. Визуализация наших данных.

	SDL_Delay(2000);// задержка чтобы посмотреть на результат


	// Очищаем память - уничтожаем все созданные объекты
	SDL_DestroyTexture(tex);
	SDL_DestroyRenderer(ren);
	SDL_DestroyWindow(win);
	SDL_Quit(); // завершаем работу с СДЛ
*/

	return 0;
}
