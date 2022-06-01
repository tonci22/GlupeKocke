#include <iostream>
#include <fstream>
#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/Audio.hpp>
using namespace std;
using namespace sf;

Texture pozadina;
Sprite s_pozadina;
Text SCORE, postotak_pogodjenih_kvadrati, BONUS, Resume, Level, Stats, Quit, Max_br_protivnika, Smedji_pogodjeni, Zuti_pogodjeni, Plavi_pogodjeni, Pogodjeni, Level1, Level2, Level3;
int multiplier = 30;	//default 30
float trci = 0.30*multiplier, mici_protivnika = 0.05*multiplier;
float pitch_quack = 1;
int brojac_kvadrati = 0, pogodjeni_protivnik_sve_boje = 0, pogodjeni_protivnik_plavi = 0, pogodjeni_protivnik_zuti = 0,
pogodjeni_protivnik_smedji = 0, ukupan_br_kvadrati = 0, palo_kvadrati = 0, score = 0, bonus_combo = 0, postotak = 0, prozirnost = 255;

//BROJ PROTIVNIKA
int broj_protivnika_bili = 10, broj_protivnika_zuti = 5, broj_protivnika_plavi = 2, broj_protivnika_smedji = 5;
int broj_protivnika = broj_protivnika_bili + broj_protivnika_zuti + broj_protivnika_smedji + broj_protivnika_plavi;
// VRIME ZA BROJIT
Clock bonus, bonus_timer, prikaz_lvl;

bool game_pause = false, provjera_stats = false, provjera_mecanje = false, lvl1 = true, lvl2 = false, lvl3 = false, pr_odabira = false, kek = true;

RectangleShape *protivnik = new RectangleShape[broj_protivnika];	//DINAMICKA ALOKACIJA ZBOG KASNIJE PROMJENE BROJA PROTIVNIKA U LVLIMA

//IF PROVJERA DALI SE DODIRUJU KVADRATI 
static bool provjera_dodira(RectangleShape &igrac, RectangleShape &protivnik) {
	if (igrac.getPosition().x - 21 <= protivnik.getPosition().x && igrac.getPosition().x + 21 >= protivnik.getPosition().x &&
		igrac.getPosition().y <= protivnik.getPosition().y + 21 && igrac.getPosition().y + 21 >= protivnik.getPosition().y) 
	{
		protivnik.setPosition(rand() % 1280, rand() % 720 - 800);
		return true;
	}
	else return false;
}

int main() 
{
	srand(time(NULL));
	rand();
	Vector2f screenDimenzije(1280, 720);
	RenderWindow punistra(VideoMode(screenDimenzije.x, screenDimenzije.y), "GLUPE KOCKE", Style::Default);
	punistra.setFramerateLimit(144);
	//punistra.setVerticalSyncEnabled(true);	//previse CPU-a zauzima iako je puno gladje nego setframerate
	pozadina.loadFromFile("slike/pozadina.jpg");
	
	s_pozadina.setTexture(pozadina);
	s_pozadina.setScale(0.8, 0.8);

	Image ikona;
	ikona.loadFromFile("slike/main_smanjeno.png");
	punistra.setIcon(32, 32, ikona.getPixelsPtr());

	SoundBuffer quack;
	quack.loadFromFile("Quack_Sound_Effect.wav");
	Sound Quack;
	Quack.setVolume(30);
	Quack.setBuffer(quack);

	SoundBuffer metallica;
	metallica.loadFromFile("Metallica - Master of Puppets (Banjo cover).ogg");
	Sound Metallica;
	Metallica.setVolume(30);
	Metallica.setBuffer(metallica);
	Metallica.setLoop(true);

	SoundBuffer tri_case;
	tri_case.loadFromFile("Milica Todorovic - Tri case - (Official Video 2013) HD.ogg");
	Sound Tri_case;
	Tri_case.setVolume(30);
	Tri_case.setBuffer(tri_case);
	Tri_case.setLoop(true);

	RectangleShape igrac(Vector2f(20, 20));
	igrac.setPosition(screenDimenzije.x*0.5, screenDimenzije.y);
	igrac.setFillColor(Color::Red);
	igrac.setOutlineThickness(1);

	RectangleShape game_pause_pozadina(Vector2f(1280,720));
	game_pause_pozadina.setFillColor(Color(0, 0, 0, 150));


	//INICIJALIZACIJA RANDOM KVADRATI I NJIHOVIH SVOJSTAVA
	for (int i = 0; i < broj_protivnika; i++) {
		if (i >= 0 && i < broj_protivnika_bili) {	//DEFAULT KOCKE
			protivnik[i].setSize(Vector2f(20, 20));
			protivnik[i].setPosition(rand() % 1280, rand() % 720 - 800);
			protivnik[i].setFillColor(Color::White);
			protivnik[i].setOutlineColor(Color::Black);
			protivnik[i].setOutlineThickness(1);
		}
		if (i >= broj_protivnika_bili && i < broj_protivnika_bili + broj_protivnika_plavi) {	//PLAVI - BRZI
			protivnik[i].setSize(Vector2f(10, 10));
			protivnik[i].setPosition(rand() % 1280, rand() % 720 - 800);
			protivnik[i].setFillColor(Color::Blue);
			protivnik[i].setOutlineColor(Color::Black);
			protivnik[i].setOutlineThickness(1);
		}
		if (i >= broj_protivnika_bili + broj_protivnika_plavi && i < broj_protivnika_bili + broj_protivnika_plavi + broj_protivnika_smedji) {	//SMEDJA - RAND LOKACIJE
			protivnik[i].setSize(Vector2f(15, 15));
			protivnik[i].setPosition(rand() % 1280, rand() % 720 - 800);
			protivnik[i].setFillColor(Color(165, 42, 42, 255));
			protivnik[i].setOutlineColor(Color::Black);
			protivnik[i].setOutlineThickness(1);
		}
		if (i >= broj_protivnika_bili + broj_protivnika_plavi + broj_protivnika_smedji && i < broj_protivnika) {	//ZUTA - BIZI OD TEBE
			protivnik[i].setSize(Vector2f(15, 15));
			protivnik[i].setPosition(rand() % 1280, rand() % 720 - 800);
			protivnik[i].setFillColor(Color::Yellow);
			protivnik[i].setOutlineColor(Color::Black);
			protivnik[i].setOutlineThickness(1);
		}
	}

	//POSTAVLJANJE FONTOVA I KARAKTERISTIKA TEKSTA U GEJMU I IZVAN
	Font font;
	font.loadFromFile("belwe-bd-bt-bold.ttf");
	SCORE.setFont(font);
	SCORE.setPosition(Vector2f(0, -10));
	SCORE.setCharacterSize(50);
	SCORE.setColor(Color::Black);
	SCORE.Bold;

	BONUS.setFont(font);
	BONUS.setPosition(Vector2f(40, 100));
	BONUS.setCharacterSize(40);
	BONUS.setColor(Color::Black);
	BONUS.Bold;

	Resume.setFont(font);
	Resume.setPosition(Vector2f(screenDimenzije.x / 2, screenDimenzije.y / 2 - 140));
	Resume.setOrigin(Vector2f(150,  0));
	Resume.setCharacterSize(60);
	Resume.setColor(Color::Black);
	Resume.setOutlineColor(Color::White);
	Resume.setOutlineThickness(2);
	Resume.Bold;
	Resume.setString("Resume");

	Level.setFont(font);
	Level.setPosition(Vector2f(screenDimenzije.x / 2, screenDimenzije.y / 2 - 70));
	Level.setOrigin(Vector2f(150, 0));
	Level.setCharacterSize(60);
	Level.setColor(Color::Black);
	Level.setOutlineColor(Color::White);
	Level.setOutlineThickness(2);
	Level.Bold;
	Level.setString("Mecanje");

	Stats.setFont(font);
	Stats.setPosition(Vector2f(screenDimenzije.x / 2, screenDimenzije.y / 2));
	Stats.setOrigin(Vector2f(150, 0));
	Stats.setCharacterSize(60);
	Stats.setColor(Color::Black);
	Stats.setOutlineColor(Color::White);
	Stats.setOutlineThickness(2);
	Stats.Bold;
	Stats.setString("Stats");

	Quit.setFont(font);
	Quit.setPosition(Vector2f(screenDimenzije.x / 2, screenDimenzije.y / 2 + 70));
	Quit.setOrigin(Vector2f(150, 0));
	Quit.setCharacterSize(60);
	Quit.setColor(Color::Black);
	Quit.setOutlineColor(Color::White);
	Quit.setOutlineThickness(2);
	Quit.Bold;
	Quit.setString("Quit");

	Max_br_protivnika.setFont(font);
	Max_br_protivnika.setPosition(Vector2f(screenDimenzije.x / 2 - 100, screenDimenzije.y / 2 - 200));
	Max_br_protivnika.setOrigin(Vector2f(150, 0));
	Max_br_protivnika.setCharacterSize(50);
	Max_br_protivnika.setColor(Color::Black);
	Max_br_protivnika.setOutlineColor(Color::White);
	Max_br_protivnika.setOutlineThickness(2);
	Max_br_protivnika.Bold;

	Plavi_pogodjeni.setFont(font);
	Plavi_pogodjeni.setPosition(Vector2f(screenDimenzije.x / 2 - 100, screenDimenzije.y / 2 - 150));
	Plavi_pogodjeni.setOrigin(Vector2f(150, 0));
	Plavi_pogodjeni.setCharacterSize(50);
	Plavi_pogodjeni.setColor(Color::Black);
	Plavi_pogodjeni.setOutlineColor(Color::White);
	Plavi_pogodjeni.setOutlineThickness(2);
	Plavi_pogodjeni.Bold;
	
	Zuti_pogodjeni.setFont(font);
	Zuti_pogodjeni.setPosition(Vector2f(screenDimenzije.x / 2 - 100, screenDimenzije.y / 2 - 100));
	Zuti_pogodjeni.setOrigin(Vector2f(150, 0));
	Zuti_pogodjeni.setCharacterSize(50);
	Zuti_pogodjeni.setColor(Color::Black);
	Zuti_pogodjeni.setOutlineColor(Color::White);
	Zuti_pogodjeni.setOutlineThickness(2);
	Zuti_pogodjeni.Bold;
	
	Smedji_pogodjeni.setFont(font);
	Smedji_pogodjeni.setPosition(Vector2f(screenDimenzije.x / 2 - 100, screenDimenzije.y / 2 - 50));
	Smedji_pogodjeni.setOrigin(Vector2f(150, 0));
	Smedji_pogodjeni.setCharacterSize(50);
	Smedji_pogodjeni.setColor(Color::Black);
	Smedji_pogodjeni.setOutlineColor(Color::White);
	Smedji_pogodjeni.setOutlineThickness(2);
	Smedji_pogodjeni.Bold;
	
	Pogodjeni.setFont(font);
	Pogodjeni.setPosition(Vector2f(screenDimenzije.x / 2 - 100, screenDimenzije.y / 2));
	Pogodjeni.setOrigin(Vector2f(150, 0));
	Pogodjeni.setCharacterSize(50);
	Pogodjeni.setColor(Color::Black);
	Pogodjeni.setOutlineColor(Color::White);
	Pogodjeni.setOutlineThickness(2);
	Pogodjeni.Bold;

	Level1.setFont(font);
	Level1.setPosition(Vector2f(screenDimenzije.x / 2, screenDimenzije.y / 2 - 140));
	Level1.setOrigin(Vector2f(150, 0));
	Level1.setCharacterSize(50);
	Level1.setColor(Color::Black);
	Level1.setOutlineColor(Color::White);
	Level1.setOutlineThickness(2);
	Level1.Bold;
	Level1.setString("Level 1");

	Level2.setFont(font);
	Level2.setPosition(Vector2f(screenDimenzije.x / 2, screenDimenzije.y / 2 - 70));
	Level2.setOrigin(Vector2f(150, 0));
	Level2.setCharacterSize(50);
	Level2.setColor(Color::Black);
	Level2.setOutlineColor(Color::White);
	Level2.setOutlineThickness(2);
	Level2.Bold;
	Level2.setString("Level 2");

	Level3.setFont(font);
	Level3.setPosition(Vector2f(screenDimenzije.x / 2, screenDimenzije.y / 2));
	Level3.setOrigin(Vector2f(150, 0));
	Level3.setCharacterSize(50);
	Level3.setColor(Color::Black);
	Level3.setOutlineColor(Color::White);
	Level3.setOutlineThickness(2);
	Level3.Bold;
	Level3.setString("Level 3");
	
	CircleShape odabir_izbornika(15, 3);
	odabir_izbornika.setFillColor(Color::Blue);
	odabir_izbornika.setOutlineColor(Color::White);
	odabir_izbornika.setOutlineThickness(2);
	odabir_izbornika.rotate(-30);
	odabir_izbornika.setPosition(Resume.getPosition().x - 200, Resume.getPosition().y + 35);

	CircleShape odabir_izbornika1(15, 3);
	odabir_izbornika1.setFillColor(Color::Blue);
	odabir_izbornika1.setOutlineColor(Color::White);
	odabir_izbornika1.setOutlineThickness(2);
	odabir_izbornika1.rotate(-30);
	odabir_izbornika1.setPosition(Resume.getPosition().x - 200, Resume.getPosition().y + 35);


	postotak_pogodjenih_kvadrati.setFont(font);
	postotak_pogodjenih_kvadrati.setPosition(screenDimenzije.x-230, screenDimenzije.y-33);
	postotak_pogodjenih_kvadrati.setColor(Color::Black);

	Metallica.play();

	while (punistra.isOpen()) 
	{
		
		Event dogadjaj;
		while (punistra.pollEvent(dogadjaj)) 
		{
			switch (dogadjaj.type) {
				case Event::Closed:
					delete[] protivnik;	//DEALOKACIJA PROTIVNIKA KAD LIVAS GAME
					protivnik = NULL;
					punistra.close();
					case Event::KeyPressed:
						if (dogadjaj.key.code == Keyboard::Space || dogadjaj.key.code == Keyboard::Escape) {
							if(!provjera_stats && !provjera_mecanje)	//DA MI SE VRATI U MAIN MENU (KAD JE GAME_PAUSE == TRUE)
								game_pause = !game_pause;	//PAUZIRANJE/UNPAUZIRANJE GEJMA
							provjera_stats = false;
							provjera_mecanje = false;
							odabir_izbornika1.setPosition(Resume.getPosition().x - 200, Resume.getPosition().y + 35);
							Metallica.pause();
						} 
						if (dogadjaj.key.code == Keyboard::Return && game_pause && odabir_izbornika.getPosition().y==(Resume.getPosition().y + 35) && !provjera_stats && !provjera_mecanje) game_pause = false;
						if (dogadjaj.key.code == Keyboard::Return && game_pause && odabir_izbornika.getPosition().y == (Resume.getPosition().y + 35 +210) && !provjera_stats && !provjera_mecanje) punistra.close();
						if (dogadjaj.key.code == Keyboard::Down && game_pause) {		//odabir_izbornika.setPosition(Resume.getPosition().x - 200, Resume.getPosition().y + 35);
							if(!provjera_mecanje) odabir_izbornika.move(0, 70);
							if (odabir_izbornika.getPosition().y > Resume.getPosition().y + 35 +210 && !provjera_mecanje)
								odabir_izbornika.setPosition(Resume.getPosition().x - 200, Resume.getPosition().y + 35);
							if (provjera_mecanje) {
								odabir_izbornika1.move(0, 70);
								if (odabir_izbornika1.getPosition().y > Resume.getPosition().y + 35 + 140)
									odabir_izbornika1.setPosition(Resume.getPosition().x - 200, Resume.getPosition().y + 35);
							}	
						}
						if (dogadjaj.key.code == Keyboard::Up && game_pause) {
							if(!provjera_mecanje) odabir_izbornika.move(0, -70);
							if (odabir_izbornika.getPosition().y < Resume.getPosition().y + 35 && !provjera_mecanje) 
								odabir_izbornika.setPosition(Resume.getPosition().x - 200, Resume.getPosition().y + 35 +210);
							if (provjera_mecanje) {
								odabir_izbornika1.move(0, -70);
								if (odabir_izbornika1.getPosition().y < Resume.getPosition().y + 35)
									odabir_izbornika1.setPosition(Resume.getPosition().x - 200, Resume.getPosition().y + 35 + 140);
							}
						} 

						//AKO KLIKNE NA STATS OTVORI SE 'STATS'
						if (dogadjaj.key.code == Keyboard::Return && game_pause && odabir_izbornika.getPosition().y == (Resume.getPosition().y + 35 + 140)) 
							provjera_stats = true;

						//AKO IZADJE IZ 'STATS'
						if (dogadjaj.key.code == Keyboard::Escape && provjera_stats)
							provjera_stats = false;

						//AKO KLIKNE NA MECANJE OTVORI SE 'MECANJE'
						if (dogadjaj.key.code == Keyboard::Return && game_pause && odabir_izbornika.getPosition().y == (Resume.getPosition().y + 35 + 70))
							provjera_mecanje = true;

						//AKO IZADJE IZ 'MECANJA'
						if (dogadjaj.key.code == Keyboard::Escape && provjera_mecanje)
							provjera_mecanje = false;

						//NE PITAJ
						if (dogadjaj.key.code == Keyboard::Return && game_pause && provjera_mecanje && odabir_izbornika1.getPosition().y == (Resume.getPosition().y + 35) && kek) {
							odabir_izbornika1.setPosition(Resume.getPosition().x - 200, Resume.getPosition().y + 34);
							kek = false;
						}
						if (game_pause && !provjera_mecanje) kek = true;

						//ODABIR LVL-A
						if (dogadjaj.key.code == Keyboard::Return && game_pause && provjera_mecanje && odabir_izbornika1.getPosition().y == (Resume.getPosition().y + 35)) {
							odabir_izbornika1.setPosition(Resume.getPosition().x - 200, Resume.getPosition().y + 35);
							lvl1 = true;
							lvl2 = false;
							lvl3 = false;
							provjera_mecanje = false;
							igrac.setPosition(screenDimenzije.x*0.5, screenDimenzije.y);
							score = 0;
							postotak = 0;
							palo_kvadrati = 0;
							ukupan_br_kvadrati = 0;
							pogodjeni_protivnik_plavi = 0;
							pogodjeni_protivnik_smedji = 0;
							pogodjeni_protivnik_zuti = 0;
							pogodjeni_protivnik_sve_boje = 0;
						}
						if (dogadjaj.key.code == Keyboard::Return && game_pause && provjera_mecanje && odabir_izbornika1.getPosition().y == (Resume.getPosition().y + 35 + 70)) {
							lvl1 = false;
							lvl2 = true;
							lvl3 = false;
							provjera_mecanje = false;
							igrac.setPosition(screenDimenzije.x*0.5, screenDimenzije.y);
							score = 0;
							postotak = 0;
							palo_kvadrati = 0;
							ukupan_br_kvadrati = 0;
							pogodjeni_protivnik_plavi = 0;
							pogodjeni_protivnik_smedji = 0;
							pogodjeni_protivnik_zuti = 0;
							pogodjeni_protivnik_sve_boje = 0;
						}
						if (dogadjaj.key.code == Keyboard::Return && game_pause && provjera_mecanje && odabir_izbornika1.getPosition().y == (Resume.getPosition().y + 35 + 140)) {
							lvl1 = false;
							lvl2 = false;
							lvl3 = true;
							provjera_mecanje = false;
							igrac.setPosition(screenDimenzije.x*0.5, screenDimenzije.y);
							score = 0;
							postotak = 0;
							palo_kvadrati = 0;
							ukupan_br_kvadrati = 0;
							pogodjeni_protivnik_plavi = 0;
							pogodjeni_protivnik_smedji = 0;
							pogodjeni_protivnik_zuti = 0;
							pogodjeni_protivnik_sve_boje = 0;
						}		
			}
		}
		
		if (game_pause && (Tri_case.getStatus() == Tri_case.Stopped || Tri_case.getStatus() == Tri_case.Paused)) //ODE SAN SE JEBA (AKO MAKNEN STOPPED ILI PAUSE ONDA CE MI STALNO POCINJAT I NIKAKO SE MAKNUT OD POCETKA PISME, OVAKO CE UVIK SVIRA NEBITNO U KOJEN SAN MENIJU KAD JE GAME PAUZIRAN)
			Tri_case.play();

		if (Metallica.getStatus() == Metallica.Paused && !game_pause) {
			Metallica.play();	//NASTAVLJA MUZIKU KAD SE UNPAUZIRA GEJM
			Tri_case.pause();
		}

		punistra.clear();
		punistra.draw(s_pozadina);

		//MICANJE IGRACA
		if (Keyboard::isKeyPressed(Keyboard::LShift)) trci = 25;
		else trci = 0.30*multiplier;
		if (!game_pause) {
			if (Keyboard::isKeyPressed(Keyboard::Right)) igrac.move(trci, 0);
			if (Keyboard::isKeyPressed(Keyboard::Left)) igrac.move(-trci, 0);
			if (Keyboard::isKeyPressed(Keyboard::Up)) igrac.move(0, -trci);
			if (Keyboard::isKeyPressed(Keyboard::Down)) igrac.move(0, trci);
		}
				
		//DA MI NE GRE IZVAN PUNISTRE
		if (igrac.getPosition().x + 20 > screenDimenzije.x) igrac.setPosition(screenDimenzije.x - 21, igrac.getPosition().y);
		if (igrac.getPosition().y + 20 > screenDimenzije.y) igrac.setPosition(igrac.getPosition().x, screenDimenzije.y - 21);
		if (igrac.getPosition().x < 0) igrac.setPosition(1, igrac.getPosition().y);
		if (igrac.getPosition().y < 0) igrac.setPosition(igrac.getPosition().x, 1);

		for (int i = 0; i < 45; i++) {///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			if (lvl2) {	//LEVEL 2
				lvl2 = false;
				delete[] protivnik;
				broj_protivnika_bili = 7;
				broj_protivnika_zuti = 5;
				broj_protivnika_plavi = 4;
				broj_protivnika_smedji = 5;
				broj_protivnika = broj_protivnika_bili + broj_protivnika_zuti + broj_protivnika_smedji + broj_protivnika_plavi;
				protivnik = new RectangleShape[broj_protivnika];

				for (int protivnik_kockica = 0; protivnik_kockica < broj_protivnika; protivnik_kockica++) {	//UCINIT FUNKCIJU OD OVEGA SMECA I SVE RESETIRAT AKO JE NOVI LVL
					if (protivnik_kockica >= 0 && protivnik_kockica < broj_protivnika_bili) {	//DEFAULT KOCKE
						protivnik[protivnik_kockica].setSize(Vector2f(20, 20));
						protivnik[protivnik_kockica].setPosition(rand() % 1280, rand() % 720 - 1800);
						protivnik[protivnik_kockica].setFillColor(Color::White);
						protivnik[protivnik_kockica].setOutlineColor(Color::Black);
						protivnik[protivnik_kockica].setOutlineThickness(1);
					}
					if (protivnik_kockica >= broj_protivnika_bili && protivnik_kockica < broj_protivnika_bili + broj_protivnika_plavi) {	//PLAVI - BRZI
						protivnik[protivnik_kockica].setSize(Vector2f(10, 10));
						protivnik[protivnik_kockica].setPosition(rand() % 1280, rand() % 720 - 1800);
						protivnik[protivnik_kockica].setFillColor(Color::Blue);
						protivnik[protivnik_kockica].setOutlineColor(Color::Black);
						protivnik[protivnik_kockica].setOutlineThickness(1);
					}
					if (protivnik_kockica >= broj_protivnika_bili + broj_protivnika_plavi && protivnik_kockica < broj_protivnika_bili + broj_protivnika_plavi + broj_protivnika_smedji) {	//SMEDJA - RAND LOKACIJE
						protivnik[protivnik_kockica].setSize(Vector2f(15, 15));
						protivnik[protivnik_kockica].setPosition(rand() % 1280, rand() % 720 - 1800);
						protivnik[protivnik_kockica].setFillColor(Color(165, 42, 42, 255));
						protivnik[protivnik_kockica].setOutlineColor(Color::Black);
						protivnik[protivnik_kockica].setOutlineThickness(1);
					}
					if (protivnik_kockica >= broj_protivnika_bili + broj_protivnika_plavi + broj_protivnika_smedji && protivnik_kockica < broj_protivnika) {	//ZUTA - BIZI OD TEBE
						protivnik[protivnik_kockica].setSize(Vector2f(15, 15));
						protivnik[protivnik_kockica].setPosition(rand() % 1280, rand() % 720 - 1800);
						protivnik[protivnik_kockica].setFillColor(Color::Yellow);
						protivnik[protivnik_kockica].setOutlineColor(Color::Black);
						protivnik[protivnik_kockica].setOutlineThickness(1);
					}
				}
			}
			else if (lvl3) {	//LEVEL 3
				lvl3 = false;	//PROMJENA STANJA DA SE IF SAMO 1 PUT IZVRSI
				delete[] protivnik;
				broj_protivnika_bili = 10;
				broj_protivnika_zuti = 7;
				broj_protivnika_plavi = 6;
				broj_protivnika_smedji = 7;
				broj_protivnika = broj_protivnika_bili + broj_protivnika_zuti + broj_protivnika_smedji + broj_protivnika_plavi;
				protivnik = new RectangleShape[broj_protivnika];
				for (int protivnik_kockica = 0; protivnik_kockica < broj_protivnika; protivnik_kockica++) {	//UCINIT FUNKCIJU OD OVEGA SMECA DA BUDE MILIJUN LINIJA MANJE JEBATE BOG
					if (protivnik_kockica >= 0 && protivnik_kockica < broj_protivnika_bili) {	//DEFAULT KOCKE
						protivnik[protivnik_kockica].setSize(Vector2f(20, 20));
						protivnik[protivnik_kockica].setPosition(rand() % 1280, rand() % 720 - 1800);
						protivnik[protivnik_kockica].setFillColor(Color::White);
						protivnik[protivnik_kockica].setOutlineColor(Color::Black);
						protivnik[protivnik_kockica].setOutlineThickness(1);
					}
					if (protivnik_kockica >= broj_protivnika_bili && protivnik_kockica < broj_protivnika_bili + broj_protivnika_plavi) {	//PLAVI - BRZI
						protivnik[protivnik_kockica].setSize(Vector2f(10, 10));
						protivnik[protivnik_kockica].setPosition(rand() % 1280, rand() % 720 - 1800);
						protivnik[protivnik_kockica].setFillColor(Color::Blue);
						protivnik[protivnik_kockica].setOutlineColor(Color::Black);
						protivnik[protivnik_kockica].setOutlineThickness(1);
					}
					if (protivnik_kockica >= broj_protivnika_bili + broj_protivnika_plavi && protivnik_kockica < broj_protivnika_bili + broj_protivnika_plavi + broj_protivnika_smedji) {	//SMEDJA - RAND LOKACIJE
						protivnik[protivnik_kockica].setSize(Vector2f(15, 15));
						protivnik[protivnik_kockica].setPosition(rand() % 1280, rand() % 720 - 1800);
						protivnik[protivnik_kockica].setFillColor(Color(165, 42, 42, 255));
						protivnik[protivnik_kockica].setOutlineColor(Color::Black);
						protivnik[protivnik_kockica].setOutlineThickness(1);
					}
					if (protivnik_kockica >= broj_protivnika_bili + broj_protivnika_plavi + broj_protivnika_smedji && protivnik_kockica < broj_protivnika) {	//ZUTA - BIZI OD TEBE
						protivnik[protivnik_kockica].setSize(Vector2f(15, 15));
						protivnik[protivnik_kockica].setPosition(rand() % 1280, rand() % 720 - 1800);
						protivnik[protivnik_kockica].setFillColor(Color::Yellow);
						protivnik[protivnik_kockica].setOutlineColor(Color::Black);
						protivnik[protivnik_kockica].setOutlineThickness(1);
					}
				}
				
			}
			else if (lvl1) {	//LEVEL 1
				lvl1 = false;
				delete[] protivnik;
				broj_protivnika_bili = 5;
				broj_protivnika_zuti = 3;
				broj_protivnika_plavi = 2;
				broj_protivnika_smedji = 3;
				broj_protivnika = broj_protivnika_bili + broj_protivnika_zuti + broj_protivnika_smedji + broj_protivnika_plavi;
				protivnik = new RectangleShape[broj_protivnika];
				for (int protivnik_kockica = 0; protivnik_kockica < broj_protivnika; protivnik_kockica++) {	//UCINIT FUNKCIJU OD OVEGA SMECA I SVE RESETIRAT AKO JE NOVI LVL
					if (protivnik_kockica >= 0 && protivnik_kockica < broj_protivnika_bili) {	//DEFAULT KOCKE
						protivnik[protivnik_kockica].setSize(Vector2f(20, 20));
						protivnik[protivnik_kockica].setPosition(rand() % 1280, rand() % 720 - 1800);
						protivnik[protivnik_kockica].setFillColor(Color::White);
						protivnik[protivnik_kockica].setOutlineColor(Color::Black);
						protivnik[protivnik_kockica].setOutlineThickness(1);
					}
					if (protivnik_kockica >= broj_protivnika_bili && protivnik_kockica < broj_protivnika_bili + broj_protivnika_plavi) {	//PLAVI - BRZI
						protivnik[protivnik_kockica].setSize(Vector2f(10, 10));
						protivnik[protivnik_kockica].setPosition(rand() % 1280, rand() % 720 - 1800);
						protivnik[protivnik_kockica].setFillColor(Color::Blue);
						protivnik[protivnik_kockica].setOutlineColor(Color::Black);
						protivnik[protivnik_kockica].setOutlineThickness(1);
					}
					if (protivnik_kockica >= broj_protivnika_bili + broj_protivnika_plavi && protivnik_kockica < broj_protivnika_bili + broj_protivnika_plavi + broj_protivnika_smedji) {	//SMEDJA - RAND LOKACIJE
						protivnik[protivnik_kockica].setSize(Vector2f(15, 15));
						protivnik[protivnik_kockica].setPosition(rand() % 1280, rand() % 720 - 1800);
						protivnik[protivnik_kockica].setFillColor(Color(165, 42, 42, 255));
						protivnik[protivnik_kockica].setOutlineColor(Color::Black);
						protivnik[protivnik_kockica].setOutlineThickness(1);
					}
					if (protivnik_kockica >= broj_protivnika_bili + broj_protivnika_plavi + broj_protivnika_smedji && protivnik_kockica < broj_protivnika) {	//ZUTA - BIZI OD TEBE
						protivnik[protivnik_kockica].setSize(Vector2f(15, 15));
						protivnik[protivnik_kockica].setPosition(rand() % 1280, rand() % 720 - 1800);
						protivnik[protivnik_kockica].setFillColor(Color::Yellow);
						protivnik[protivnik_kockica].setOutlineColor(Color::Black);
						protivnik[protivnik_kockica].setOutlineThickness(1);
					}
				}
			}
		}
		for (int i = 0; i < broj_protivnika; i++) {
			
			for (int j = 0; j < broj_protivnika && i != j; j++) {//DA MI SE RAND KVADRATI NE DODIRUJU PREVISE ILI NIKAKO	(osin zute)
				if (protivnik[i].getPosition().x - 50 <= protivnik[j].getPosition().x && protivnik[i].getPosition().x + 50 >= protivnik[j].getPosition().x &&
					protivnik[i].getPosition().y <= protivnik[j].getPosition().y + 50 && protivnik[i].getPosition().y + 50 >= protivnik[j].getPosition().y && !game_pause && !(i >= broj_protivnika_bili + broj_protivnika_plavi + broj_protivnika_smedji && i < broj_protivnika)) protivnik[i].move(mici_protivnika - 5.8, 0); 
			}

			

			//AKO PROTIVNIK ODE IZVAN PROZORA POJAVIT CE SE NA DRUGU STRANU PROZORA
			if (protivnik[i].getPosition().x > screenDimenzije.x) 
				protivnik[i].setPosition(Vector2f(0, protivnik[i].getPosition().y));

			if(protivnik[i].getPosition().x < 0) 
				protivnik[i].setPosition(Vector2f(screenDimenzije.x, protivnik[i].getPosition().y));

			if (protivnik[i].getPosition().y < 0 && protivnik[i].getPosition().y > -2 && !game_pause) 
				brojac_kvadrati++;	//broji rand kvadrate (izgleda da ako stavin == -1 ne provjerava dobro)
			
			//POMICANJE RAND KVADRATA ZA 'MICI_KOCKE'
			if (!game_pause) {
				if (i >= 0 && i < broj_protivnika_bili) 
					protivnik[i].move(0, mici_protivnika);

				if (i >= broj_protivnika_bili && i < broj_protivnika_bili + broj_protivnika_plavi) 
					protivnik[i].move(0, mici_protivnika + 1);

				if (i >= broj_protivnika_bili + broj_protivnika_plavi && i < broj_protivnika_bili + broj_protivnika_plavi + broj_protivnika_smedji) 
					protivnik[i].move(rand() % 5 - 2, mici_protivnika);

				if (i >= broj_protivnika_bili + broj_protivnika_plavi + broj_protivnika_smedji && i < broj_protivnika) 
					protivnik[i].move(0, mici_protivnika);
			}
			if (i >= broj_protivnika_bili + broj_protivnika_plavi + broj_protivnika_smedji && i < broj_protivnika && protivnik[i].getPosition().y > screenDimenzije.y / 3) {
				protivnik[i].setFillColor(Color(255, 255, 0, 30));
				protivnik[i].setOutlineColor(Color(0, 0, 0, 10));
			}
			
			punistra.draw(protivnik[i]);			//CRTANJE RAND KVADRATA
			
			//PONOVNO POZICIONIRANJE RAND KVADRATA NA VRH EKRANA
			if (protivnik[i].getPosition().y > screenDimenzije.y && !game_pause) {
				protivnik[i].setPosition(rand() % 1280, rand() % 720 - 800);
				palo_kvadrati++;
				if (i >= broj_protivnika_bili + broj_protivnika_plavi + broj_protivnika_smedji && i < broj_protivnika) {
					protivnik[i].setFillColor(Color(255, 255, 0, 255));
					protivnik[i].setOutlineColor(Color(0, 0, 0, 255));
				}
			}

			if (provjera_dodira(igrac, protivnik[i])) {
				Quack.play();
				pogodjeni_protivnik_sve_boje++;	//BROJI KOLIKO SI KVADRATI TAKNU
				score += 100;			//BODOVANJE ZA SVAKI PODGODJENI RAND KVADRAT
				//DODATNI BOODVI ZA POGODJENE KVADRATE (PLAVI, SMEDJI, ZUTI, ITD...)
				if (i >= broj_protivnika_bili && i < broj_protivnika_bili + broj_protivnika_plavi) {
					score += 100;
					pogodjeni_protivnik_plavi++;
				}
				if (i >= broj_protivnika_bili + broj_protivnika_plavi && i < broj_protivnika_bili + broj_protivnika_plavi + broj_protivnika_smedji) {
					score += 50;
					pogodjeni_protivnik_smedji++;
				}
				if (i >= broj_protivnika_bili + broj_protivnika_plavi + broj_protivnika_smedji && i < broj_protivnika) {
					score += 75;
					pogodjeni_protivnik_zuti++;
				}

				//BONUS BODOVI AKO SI U ODREDJENEMU VRIMENU VISE KOCKI TAKNU LUL
				if (bonus.restart().asMilliseconds() < 500) {
					Quack.setPitch(pitch_quack+=0.08);
					bonus_combo += 50;
					score += 50;
					prozirnost = 255;
					bonus_timer.restart().Zero;
				}
				else {
					bonus_combo = 0;
					Quack.setPitch(1);
					pitch_quack = 1;
				}
			}
		}
		//VRIME SE STALNO STAVLJE NA 0 SVE DOK SE NE AKTIVIRA COMBO, ONDA JE 'bonus_combo' != 0 IDE SVE DO 0,5s 
		if (bonus_combo == 0) {
			bonus_timer.restart().Zero;	//RESTARTAN CLOCK JER AKO JE COMBO PREVELIK BONUS SE NE ISPISUJE JER PRODJE 1,5 SEC
			prozirnost = 0;
		}
			
		if (bonus_combo > 0 && !game_pause && bonus_timer.getElapsedTime().asMilliseconds() < 1500) {	//TREBA MI BONUS_TIMER JER NE UPDAJTE-A BONUS_COMBO DOK NE TAKNE NEKU KOCKU 
			prozirnost--;
			if (prozirnost == 0) prozirnost = 255;
			BONUS.setFillColor(Color(0, 0, 0, prozirnost));
			punistra.draw(BONUS);
		}
		
		ukupan_br_kvadrati = palo_kvadrati + pogodjeni_protivnik_sve_boje;
		if (brojac_kvadrati != 0 && pogodjeni_protivnik_sve_boje != 0) postotak = ((float)pogodjeni_protivnik_sve_boje / (float)palo_kvadrati) * 100;
		
		if (postotak > 100 || postotak < 0) postotak = 100;
		postotak_pogodjenih_kvadrati.setString("HIT RATE: " + to_string(postotak) + "%");
		SCORE.setString("SCORE: " + to_string(score));
		BONUS.setString(to_string(bonus_combo)+"x BONUS");
		
		punistra.draw(igrac);
		punistra.draw(postotak_pogodjenih_kvadrati);
		punistra.draw(SCORE);

			
		if (game_pause && !provjera_stats && !provjera_mecanje) {	//CRTANJE ELEMENATA KADA JE GEJM PAUZIRAN
			bonus.restart();	//KAD JE GAME PAUZIRAN ONDA JE VRIME NA 0
			punistra.draw(game_pause_pozadina);	
			punistra.draw(odabir_izbornika);
			punistra.draw(Resume);
			punistra.draw(Level);
			punistra.draw(Stats);
			punistra.draw(Quit);
			
		} 
		if (game_pause && provjera_stats) {	//CRTANJE STATSOVA		
			punistra.draw(game_pause_pozadina);
			Stats.setPosition(Vector2f(screenDimenzije.x / 2, screenDimenzije.y / 2 - 300));
			Max_br_protivnika.setString("Max br. protivnika: " + to_string(ukupan_br_kvadrati));
			Plavi_pogodjeni.setString("Br. PLAVIH pogodjeno: " + to_string(pogodjeni_protivnik_plavi));
			Zuti_pogodjeni.setString("Br. ZUTIH pogodjeno: " + to_string(pogodjeni_protivnik_zuti));
			Smedji_pogodjeni.setString("Br. SMEDJIH pogodjeno: " + to_string(pogodjeni_protivnik_smedji));
			Pogodjeni.setString("Br. SVIH pogodjeno: " + to_string(pogodjeni_protivnik_sve_boje));
			punistra.draw(Stats);
			punistra.draw(Max_br_protivnika);
			punistra.draw(Plavi_pogodjeni);
			punistra.draw(Zuti_pogodjeni);
			punistra.draw(Smedji_pogodjeni);
			punistra.draw(Pogodjeni);
		}
		else if (game_pause && provjera_mecanje) {
			if(pr_odabira) odabir_izbornika1.setPosition(Resume.getPosition().x - 200, Resume.getPosition().y + 35);
			pr_odabira = false;
			Level.setPosition(Vector2f(screenDimenzije.x / 2 - 40, screenDimenzije.y / 2 - 300));
			punistra.draw(game_pause_pozadina);
			punistra.draw(Level);
			punistra.draw(Level1);
			punistra.draw(Level2);
			punistra.draw(Level3);
			punistra.draw(odabir_izbornika1);
		}
		else {
			Stats.setPosition(Vector2f(screenDimenzije.x / 2, screenDimenzije.y / 2));
			Level.setPosition(Vector2f(screenDimenzije.x / 2, screenDimenzije.y / 2 - 70));
			pr_odabira = true;
		}
		punistra.display();
	}
	return 0;
}
/*
			TODO
-BODOVI LIVO GORI	!GOTOVO!
-QUACK ZVUK KAD POGODJIS RAND KVADRAT (COMBO PITCHA ZVUK)	!GOTOVO!
-TESKI METAL BACKGROUND ZVUK	!GOTOVO!
-TESKA CAJKA KAD JE GAME PAUZIRAN	!GOTOVO!
-SVAKI RAND KVADRAT SE PONEKAD MAKNE MALO U STRANU LUL	!GOTOVO!
-AKO IH CAPAS VISE UNUTAR ODREDJENEGA VRIMENA == BONUS BODOVI	!GOTOVO!
-KOLIKO JE KVADRATI PROSLO	!GOTOVO!
-% POGODJENIH	!GOTOVO!
-VISE BOJI KOCKI (SVAKA BOJA NES ZNACI, NPR. PLAVA JE BRZA OD BILE, SMEDJA IDE U RANDOM LOKACIJE, ZUTA POSTAJE SKORO PROZIRNA)	!GOTOVO!
-VISE LVL-A	!GOTOVO!
-UCINI NORMALNI KOD JBT
*/