//// ---------------------------------------
//// Uzycie SOIL
//
//
////0. Zakladam ze juz mamy biblioteke w systemie
////
////1. Dopisanie plikow naglowkowych (NOWE)
//
//	#include <SOIL.h>
//
//
////2. Tworzenie tekstury jak poprzednio
//
//	glEnable(GL_TEXTURE_2D);
//
//	glGenTextures(1, &TextureID);
//	glBindTexture(GL_TEXTURE_2D, TextureID);
//
//
//3. Ladowanie danych z pliku graficznego  (NOWE)
//
//Uwaga: Format JPG tylko w wersji nieprogresywnej, reszta powinna normalnie dzialac
//Mozna zawsze otworzyc plik jpg ktory nie dziala i zapisac go raz jeszcze w
//programie graficznym bez zaznaczania opcji progressive (progressive browser
//display lub podobnie)
//
//
//	int width, height;
//	unsigned char* image;
//
//
//4. Najwazniejsza funkcja  (NOWE)
//
//	image = SOIL_load_image("texture1.jpg", &width, &height, 0, SOIL_LOAD_RGB);
//
//5. Dalej juz standardowo, przerzucenie danych do opengla i ustawienie parametrow tekstury
//
//	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
//
//	glGenerateMipmap(GL_TEXTURE_2D);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//
//
//6. Dobra praktyka: zwolnienie pamieci z obrazu, juz niepotrzebny, bo obraz zostal skopiowany do
//pamieci zarzadzanej przez opengl.	 (NOWE)
//
//	SOIL_free_image_data(image);
//
//
//7. Podczas wyswietlania jak poprzednio
//
//	// Wlaczanie/aktywowanie tekstur
//	glActiveTexture(GL_TEXTURE0);
//	glBindTexture(GL_TEXTURE_2D, TextureID);
//
//	// Ustawianie tzw. samplerow zwiazanych z teksturami
//
//	glUniform1i(glGetUniformLocation(program, "tex0"), 0);
//
//
//	// Generowanie obiektow na ekranie
//	glDrawArrays( GL_TRIANGLES, 0, OBJ_vertices.size() );
//
