/*
==================================================================================
cGame.cpp
==================================================================================
*/
#include "cGame.h"


cGame* cGame::pInstance = NULL;
static cTextureMgr* theTextureMgr = cTextureMgr::getInstance();
static cFontMgr* theFontMgr = cFontMgr::getInstance();
static cButtonMgr* theButtonMgr = cButtonMgr::getInstance();
static cSoundMgr* theSoundMgr = cSoundMgr::getInstance();


/*
=================================================================================
Constructor
=================================================================================
*/
cGame::cGame()
{

}
/*
=================================================================================
Singleton Design Pattern
=================================================================================
*/
cGame* cGame::getInstance()
{
	if (pInstance == NULL)
	{
		pInstance = new cGame();
	}
	return cGame::pInstance;
}


void cGame::initialise(SDL_Window* theSDLWND, SDL_Renderer* theRenderer)
{
	// Get width and height of render context
	SDL_GetRendererOutputSize(theRenderer, &renderWidth, &renderHeight);
	this->m_lastTime = high_resolution_clock::now();
	// Clear the buffer with a black background
	SDL_SetRenderDrawColor(theRenderer, 0, 0, 0, 255);
	SDL_RenderPresent(theRenderer);
	theScore = 0;

	theTextureMgr->setRenderer(theRenderer); //Texture Manager
	theFontMgr->initFontLib(); //Font Manager
	theSoundMgr->initMixer(); //Sound Manager
	theAreaClicked = { 0, 0 }; //Sets the are clicked to (0,0) - (x,y)
	
	theTextureMgr->setRenderer(theRenderer);
	theFontMgr->initFontLib();
	theSoundMgr->initMixer();

	// Store the textures
	textureName = { "ringOfFire","sharkTank","ramp","asteroid1", "asteroid2", "asteroid3", "asteroid4", "bullet","theRocket","theBackground", "grenade", "roadBackground" ,"playerCharacter"};
	texturesToUse = { "Images\\ringOfFire.png","Images\\sharkTank.png","Images\\ramp.png","Images\\asteroid1.png", "Images\\asteroid2.png", "Images\\asteroid3.png", "Images\\asteroid4.png", "Images\\bullet.png", "Images\\rocketSprite.png", "Images\\starscape1024x768.png", "Images\\grenade.png", "Images\\mainSceneBackground.png", "Images\\car.png", };
	for (int tCount = 0; tCount < textureName.size(); tCount++)
	{	
		theTextureMgr->addTexture(textureName[tCount], texturesToUse[tCount]);
	}
	//buttonStuff
	btnNameList = { "exit_btn", "instructions_btn", "load_btn", "menu_btn", "play_btn", "save_btn", "settings_btn" }; //Array of button name list
	btnTexturesToUse = { "Images/Buttons/button_exit.png", "Images/Buttons/button_instructions.png", "Images/Buttons/button_load.png", "Images/Buttons/button_menu.png", "Images/Buttons/button_play.png", "Images/Buttons/button_save.png", "Images/Buttons/button_settings.png" }; //Button texture file locations
	btnPos = { { 350, 375 }, { 350, 300 }, { 400, 300 }, { 325, 500 }, { 400, 300 }, { 740, 500 }, { 400, 300 } }; //Position of the buttons
	for (int bCount = 0; bCount < btnNameList.size(); bCount++) //For integar bcount equals 0, bcount is less than the button name list, add 1 to bcount
	{
		theTextureMgr->addTexture(btnNameList[bCount], btnTexturesToUse[bCount]); //Adds the button texture to use
		cButton * newBtn = new cButton(); //newBtn equals to new cButton()
		newBtn->setTexture(theTextureMgr->getTexture(btnNameList[bCount])); //Sets texture for the button
		newBtn->setSpritePos(btnPos[bCount]); //Sets button position
		newBtn->setSpriteDimensions(theTextureMgr->getTexture(btnNameList[bCount])->getTWidth(), theTextureMgr->getTexture(btnNameList[bCount])->getTHeight()); //Sets button dimensions
		theButtonMgr->add(btnNameList[bCount], newBtn); //Adds button
	}


	theGameState = MENU; //Sets current game state to Menu
	theBtnType = EXIT; //Sets button type to exit


	// Create textures for Game Dialogue (text)
	fontList = { "digital", "spaceAge" };
	fontsToUse = { "Fonts/digital-7.ttf", "Fonts/space age.ttf" };
	for (int fonts = 0; fonts < fontList.size(); fonts++)
	{
		theFontMgr->addFont(fontList[fonts], fontsToUse[fonts], 36);
	}
	

	// Create text Textures
	gameTextNames = { "TitleTxt", "ThanksTxt", "SeeYouTxt", "ScoreText" };
	gameTextList = { "All My Own Stunts", "Thanks for playing!", "See you again soon!", "Score: " };


	for (int text = 0; text < gameTextNames.size(); text++) //For, integar text = 0, text is less than text name size, add 1 to text
	{
		theTextureMgr->addTexture(gameTextNames[text], theFontMgr->getFont("digital")->createTextTexture(theRenderer, gameTextList[text], SOLID, { 228, 213, 238, 255 }, { 0, 0, 0, 0 })); //Adds the font type digital and have a solid clear background to it
	}
	// Load game sounds
	soundList = { "theme", "shot", "explosion"};
	soundTypes = { MUSIC, SFX, SFX };
	soundsToUse = { "Audio/youBetterCallKennyLoggins.wav", "Audio/shot007.wav", "Audio/explosion2.wav" };
	for (int sounds = 0; sounds < soundList.size(); sounds++)
	{
		theSoundMgr->add(soundList[sounds], soundsToUse[sounds], soundTypes[sounds]);
	}

	theSoundMgr->getSnd("theme")->play(-1); // -1 makes it loop

	spriteBkgd.setSpritePos({ 0, 0 });
	spriteBkgd.setTexture(theTextureMgr->getTexture("roadBackground"));
	spriteBkgd.setSpriteDimensions(theTextureMgr->getTexture("roadBackground")->getTWidth(), theTextureMgr->getTexture("roadBackground")->getTHeight());

	theRocket.setSpritePos({ 500, 550 });
	theRocket.setTexture(theTextureMgr->getTexture("playerCharacter"));
	theRocket.setSpriteDimensions(theTextureMgr->getTexture("playerCharacter")->getTWidth(), theTextureMgr->getTexture("playerCharacter")->getTHeight());
	theRocket.setRocketVelocity({ 0, 0 });

	 //Create vector array of textures

	/*for (int astro = 0; astro < 5; astro++)
	{
		theAsteroids.push_back(new cAsteroid);
		theAsteroids[astro]->setSpritePos({ 100 * (rand() % 5 + 1), 50 * (rand() % 5 + 1) });
		theAsteroids[astro]->setSpriteTranslation({ (rand() % 8 + 1), (rand() % 8 + 1) });
		int randAsteroid = rand() % 3;
		theAsteroids[astro]->setTexture(theTextureMgr->getTexture(textureName[randAsteroid]));
		theAsteroids[astro]->setSpriteDimensions(theTextureMgr->getTexture(textureName[randAsteroid])->getTWidth(), theTextureMgr->getTexture(textureName[randAsteroid])->getTHeight());
		theAsteroids[astro]->setAsteroidVelocity({ 3.0f, 3.0f });
		theAsteroids[astro]->setActive(true);
	}*/
	
	
	
	for (int counter = 0; counter < 3; counter++)
	{
		theAsteroids.push_back(new cAsteroid);
		theAsteroids[counter]->setSpritePos({ 220 * (1 + counter), 0 });
		//theAsteroids[counter]->setSpriteTranslation({ 0, 8 });
		
		randAsteroid = rand() %  3;
		//cout << counter << ": " << randAsteroid << "   ";
		laneSpeed[counter] = rand() % 8 + 1;
		theAsteroids[counter]->setTexture(theTextureMgr->getTexture(textureName[randAsteroid]));
		theAsteroids[counter]->setSpriteDimensions(theTextureMgr->getTexture(textureName[randAsteroid])->getTWidth(), theTextureMgr->getTexture(textureName[randAsteroid])->getTHeight());
	//	theAsteroids[counter]->setAsteroidVelocity({ 3.0f, 3.0f });
		theAsteroids[counter]->setActive(true);
		//theAsteroids[counter]->setSpritePos({ theAsteroids[counter]->getSpritePos().x, theAsteroids[counter]->getSpritePos().y + laneSpeed[counter] });
		
	}
	
	
	theSScore = gameTextList[3] + to_string(theScore);

}

void cGame::run(SDL_Window* theSDLWND, SDL_Renderer* theRenderer)
{
	loop = true;

	while (loop)
	{
		//We get the time that passed since the last frame
		double elapsedTime = this->getElapsedSeconds();

		loop = this->getInput(loop);
		this->update(elapsedTime);
		this->render(theSDLWND, theRenderer);
	}
}

void cGame::render(SDL_Window* theSDLWND, SDL_Renderer* theRenderer)
{ 
	SDL_RenderClear(theRenderer);
	//spriteBkgd.render(theRenderer, NULL, NULL, spriteBkgd.getSpriteScale());
	//// Render each asteroid in the vector array
	//for (int draw = 0; draw < theAsteroids.size(); draw++)
	//{
	//	theAsteroids[draw]->render(theRenderer, &theAsteroids[draw]->getSpriteDimensions(), &theAsteroids[draw]->getSpritePos(), theAsteroids[draw]->getSpriteRotAngle(), &theAsteroids[draw]->getSpriteCentre(), theAsteroids[draw]->getSpriteScale());
	//}

	////if (laneClear == true);
	////{
	//	//theAsteroids[numOfAsteroids]->render(theRenderer, &theAsteroids[numOfAsteroids]->getSpriteDimensions(), &theAsteroids[numOfAsteroids]->getSpritePos(), theAsteroids[numOfAsteroids]->getSpriteRotAngle(), &theAsteroids[numOfAsteroids]->getSpriteCentre(), theAsteroids[numOfAsteroids]->getSpriteScale());
	////} 
	//


	//// Render each bullet in the vector array
	//for (int draw = 0; draw < theBullets.size(); draw++)
	//{
	//	theBullets[draw]->render(theRenderer, &theBullets[draw]->getSpriteDimensions(), &theBullets[draw]->getSpritePos(), theBullets[draw]->getSpriteRotAngle(), &theBullets[draw]->getSpriteCentre(), theBullets[draw]->getSpriteScale());
	//}
	//// Render the Title
	//cTexture* tempTextTexture = theTextureMgr->getTexture("Title");
	//SDL_Rect pos = { 10, 10, tempTextTexture->getTextureRect().w, tempTextTexture->getTextureRect().h };
	//FPoint scale = { 1, 1 };
	//tempTextTexture->renderTexture(theRenderer, tempTextTexture->getTexture(), &tempTextTexture->getTextureRect(), &pos, scale);
	//
	////Render the Score
	//theTextureMgr->addTexture("Score", theFontMgr->getFont("spaceAge")->createTextTexture(theRenderer, theSScore.c_str(), SOLID, { 0, 255, 0, 255 }, { 0, 0, 0, 0 }));
	//cTexture* tempTextTexture2 = theTextureMgr->getTexture("Score");
	//SDL_Rect pos2 = {700, 10, tempTextTexture2->getTextureRect().w, tempTextTexture2->getTextureRect().h };
	//FPoint scale2 = { 1, 1 };
	//tempTextTexture2->renderTexture(theRenderer, tempTextTexture2->getTexture(), &tempTextTexture2->getTextureRect(), &pos2, scale2);
	

	//// Render the rocket
	//theRocket.render(theRenderer, &theRocket.getSpriteDimensions(), &theRocket.getSpritePos(), theRocket.getSpriteRotAngle(), &theRocket.getSpriteCentre(), theRocket.getSpriteScale());
	//SDL_RenderPresent(theRenderer);

	////Rendering Buttons
	//theButtonMgr->getBtn("play_btn")->render(theRenderer, &theButtonMgr->getBtn("play_btn")->getSpriteDimensions(), &theButtonMgr->getBtn("play_btn")->getSpritePos(), theButtonMgr->getBtn("play_btn")->getSpriteScale()); //Renders the play button
	//theButtonMgr->getBtn("exit_btn")->setSpritePos({ 350, 375 }); //Position of the exit button
	//theButtonMgr->getBtn("exit_btn")->render(theRenderer, &theButtonMgr->getBtn("exit_btn")->getSpriteDimensions(), &theButtonMgr->getBtn("exit_btn")->getSpritePos(), theButtonMgr->getBtn("exit_btn")->getSpriteScale()); //Renders the exit button
	//
	//if (theButtonMgr->getBtn("exit_btn")->getClicked())
	//{
	//	resetGame = false; //Reset the end check
	//	theButtonMgr->getBtn("exit_btn")->setClicked(false); //Reset the exit button
	//}
	switch (theGameState)
	{
	case MENU:
	{
		//cTexture* tempTextTexture = theTextureMgr->getTexture("Title");
		//SDL_Rect pos = { 10, 10, tempTextTexture->getTextureRect().w, tempTextTexture->getTextureRect().h };
		//FPoint scale = { 1, 1 };
		//spriteBkgd.render(theRenderer, NULL, NULL, spriteBkgd.getSpriteScale());
		////Texts render is main menu screen
		//tempTextTexture = theTextureMgr->getTexture("TitleTxt"); //Temperory txture set to Title text
		//pos = { 350, 10, tempTextTexture->getTextureRect().w, tempTextTexture->getTextureRect().h }; //Positon of the text
		//scale = { 1, 1 }; //Scale of the text
		//tempTextTexture->renderTexture(theRenderer, tempTextTexture->getTexture(), &tempTextTexture->getTextureRect(), &pos, scale); //Renders the text with the scale and position
		//tempTextTexture = theTextureMgr->getTexture("ContorlsTxt"); //Temporary texture set to Contorls text
		//pos = { 250, 100, tempTextTexture->getTextureRect().w, tempTextTexture->getTextureRect().h }; //Position of the texure
		//tempTextTexture->renderTexture(theRenderer, tempTextTexture->getTexture(), &tempTextTexture->getTextureRect(), &pos, scale); //Renders the texture with scale and position
		//tempTextTexture = theTextureMgr->getTexture("ContorlsTxt2"); //Temporary texture set to Contorls text
		//pos = { 250, 150, tempTextTexture->getTextureRect().w, tempTextTexture->getTextureRect().h }; //Position of the texure
		//tempTextTexture->renderTexture(theRenderer, tempTextTexture->getTexture(), &tempTextTexture->getTextureRect(), &pos, scale);

		spriteBkgd.render(theRenderer, NULL, NULL, spriteBkgd.getSpriteScale());
		// Render the Title
		tempTextTexture = theTextureMgr->getTexture("TitleTxt"); //Temperory txture set to Title text
		pos = { 350, 10, tempTextTexture->getTextureRect().w, tempTextTexture->getTextureRect().h }; //Positon of the text
		scale = { 1, 1 }; //Scale of the text
		tempTextTexture->renderTexture(theRenderer, tempTextTexture->getTexture(), &tempTextTexture->getTextureRect(), &pos, scale); //Renders the text with the scale and position
		tempTextTexture = theTextureMgr->getTexture("TitleTxt"); //Temporary texture set to Contorls text
		pos = { 250, 100, tempTextTexture->getTextureRect().w, tempTextTexture->getTextureRect().h }; //Position of the texure
		tempTextTexture->renderTexture(theRenderer, tempTextTexture->getTexture(), &tempTextTexture->getTextureRect(), &pos, scale); //Renders the texture with scale and position
		tempTextTexture = theTextureMgr->getTexture("TitleTxt"); //Temporary texture set to Contorls text
		pos = { 250, 150, tempTextTexture->getTextureRect().w, tempTextTexture->getTextureRect().h }; //Position of the texure
		tempTextTexture->renderTexture(theRenderer, tempTextTexture->getTexture(), &tempTextTexture->getTextureRect(), &pos, scale);




		//Rendering Buttons
		theButtonMgr->getBtn("play_btn")->render(theRenderer, &theButtonMgr->getBtn("play_btn")->getSpriteDimensions(), &theButtonMgr->getBtn("play_btn")->getSpritePos(), theButtonMgr->getBtn("play_btn")->getSpriteScale()); //Renders the play button
		theButtonMgr->getBtn("exit_btn")->setSpritePos({ 350, 375 }); //Position of the exit button
		theButtonMgr->getBtn("exit_btn")->render(theRenderer, &theButtonMgr->getBtn("exit_btn")->getSpriteDimensions(), &theButtonMgr->getBtn("exit_btn")->getSpritePos(), theButtonMgr->getBtn("exit_btn")->getSpriteScale()); //Renders the exit button
	}
	break;
	case PLAYING:
	{
		//cout << theGameState;
		//Render Title
		/*cTexture* tempTextTexture = theTextureMgr->getTexture("Title");
		SDL_Rect pos = { 10, 10, tempTextTexture->getTextureRect().w, tempTextTexture->getTextureRect().h };
		FPoint scale = { 1, 1 };*/

		//Render Background - MAYBE IDEK?
		
		/*spriteBkgd.setSpritePos({ 0, 0 });
		spriteBkgd.setTexture(theTextureMgr->getTexture("roadBackground"));
		spriteBkgd.setSpriteDimensions(theTextureMgr->getTexture("roadBackground")->getTWidth(), theTextureMgr->getTexture("roadBackground")->getTHeight());
*/
		//Render the Score
		
		///*theTextureMgr->addTexture("Score", theFontMgr->getFont("spaceAge")->createTextTexture(theRenderer, theSScore.c_str(), SOLID, { 0, 255, 0, 255 }, { 0, 0, 0, 0 }));
		//cTexture* tempTextTexture2 = theTextureMgr->getTexture("Score");
		//SDL_Rect pos2 = { 700, 10, tempTextTexture2->getTextureRect().w, tempTextTexture2->getTextureRect().h };
		//FPoint scale2 = { 1, 1 };
		//tempTextTexture2->renderTexture(theRenderer, tempTextTexture2->getTexture(), &tempTextTexture2->getTextureRect(), &pos2, scale2);*/
		//theTextureMgr->addTexture("ScoreText", theFontMgr->getFont("digital")->createTextTexture(theRenderer, theSScore.c_str(), SOLID, { 255, 0, 0, 255 }, { 0, 0, 0, 0 }));
		//tempTextTexture = theTextureMgr->getTexture("ScoreText");
		//pos = { 900, 10, tempTextTexture->getTextureRect().w, tempTextTexture->getTextureRect().h };
		//tempTextTexture->renderTexture(theRenderer, tempTextTexture->getTexture(), &tempTextTexture->getTextureRect(), &pos, scale);

		
		//Renders the exit button
		
		//theButtonMgr->getBtn("exit_btn")->setSpritePos({ 940, 575 }); //Exit button position
		//theButtonMgr->getBtn("exit_btn")->render(theRenderer, &theButtonMgr->getBtn("exit_btn")->getSpriteDimensions(), &theButtonMgr->getBtn("exit_btn")->getSpritePos(), theButtonMgr->getBtn("exit_btn")->getSpriteScale()); //Renders exit button
		////Renders the load button
		//theButtonMgr->getBtn("load_btn")->setSpritePos({ 940, 620 }); //Load button position
		//theButtonMgr->getBtn("load_btn")->render(theRenderer, &theButtonMgr->getBtn("load_btn")->getSpriteDimensions(), &theButtonMgr->getBtn("load_btn")->getSpritePos(), theButtonMgr->getBtn("load_btn")->getSpriteScale()); //Renders load button
		////Renders the save button
		//theButtonMgr->getBtn("save_btn")->setSpritePos({ 940, 695 }); //Save button position
		//theButtonMgr->getBtn("save_btn")->render(theRenderer, &theButtonMgr->getBtn("save_btn")->getSpriteDimensions(), &theButtonMgr->getBtn("save_btn")->getSpritePos(), theButtonMgr->getBtn("save_btn")->getSpriteScale()); //Renders the save button
	

		cout << theAsteroids.size();

		//for (int workPlease2 = 0; workPlease2 < theAsteroids.size(); workPlease2++)
		//{
		//	if (theAsteroids[workPlease2]->getSpritePos().y > 760)
		//	{
		//		workPlease3 = workPlease2;
		//		theAsteroids[workPlease2]->setActive(false);
		//	}
		//	theAsteroids[workPlease2]->setSpritePos({ theAsteroids[workPlease2]->getSpritePos().x, theAsteroids[workPlease2]->getSpritePos().y + laneSpeed[workPlease2] });
		//}


		////Render more obstacles 
		//if (theAsteroids.size() < 3)
		//{
		//	int obs = 3 - theAsteroids.size();
		//	cout << "obs    " << obs;
		//	for (static int workPlease = 0; workPlease < obs; workPlease++)
		//	{
		//		theAsteroids[workPlease3]->setSpritePos({ 220 * (1 + workPlease), 0 });
		//		randObs[workPlease3] = rand() % 3;
		//		int currentObs = randObs[workPlease3];
		//		theAsteroids[workPlease3]->setTexture(theTextureMgr->getTexture(textureName[currentObs]));
		//		theAsteroids[workPlease3]->setSpriteDimensions(theTextureMgr->getTexture(textureName[currentObs])->getTWidth(), theTextureMgr->getTexture(textureName[currentObs])->getTHeight());
		//		laneSpeed[workPlease] = rand() % 8 + 1;
		//		//theAsteroids[workPlease]->setSpritePos({ theAsteroids[workPlease]->getSpritePos().x, theAsteroids[workPlease]->getSpritePos().y + laneSpeed[workPlease] });
		//		theAsteroids[workPlease3]->setActive(true);
		//		currentObs = 0;
		//		
		//		//workPlease++;
		//	}
			



		//}
		//cout << asteroidIterator;
			
		for (int workPlease2 = 0; workPlease2 < theAsteroids.size(); workPlease2++)
		{
			if (theAsteroids[workPlease2]->getSpritePos().y > 760 || workPlease2 == asteroidShot)
			{
				theAsteroids[workPlease2]->setSpritePos({ 220 * (1 + workPlease2), 0 });
						randObs[workPlease2] = rand() % 3;
						int currentObs = randObs[workPlease2];
						theAsteroids[workPlease2]->setTexture(theTextureMgr->getTexture(textureName[currentObs]));
						theAsteroids[workPlease2]->setSpriteDimensions(theTextureMgr->getTexture(textureName[currentObs])->getTWidth(), theTextureMgr->getTexture(textureName[currentObs])->getTHeight());
						laneSpeed[workPlease2] = rand() % 8 + 4;
						//theAsteroids[workPlease]->setSpritePos({ theAsteroids[workPlease]->getSpritePos().x, theAsteroids[workPlease]->getSpritePos().y + laneSpeed[workPlease] });
						theAsteroids[workPlease2]->setActive(true);
						currentObs = 0;
						asteroidShot = 4;
			}
			theAsteroids[workPlease2]->setSpritePos({ theAsteroids[workPlease2]->getSpritePos().x, theAsteroids[workPlease2]->getSpritePos().y + laneSpeed[workPlease2] });
		}


		
		
		spriteBkgd.render(theRenderer, NULL, NULL, spriteBkgd.getSpriteScale());
		tempTextTexture = theTextureMgr->getTexture("TitleTxt");
		pos = { 10, 10, tempTextTexture->getTextureRect().w, tempTextTexture->getTextureRect().h };
		tempTextTexture->renderTexture(theRenderer, tempTextTexture->getTexture(), &tempTextTexture->getTextureRect(), &pos, scale);
		theButtonMgr->getBtn("exit_btn")->setSpritePos({ 740, 650 });
		theButtonMgr->getBtn("exit_btn")->render(theRenderer, &theButtonMgr->getBtn("exit_btn")->getSpriteDimensions(), &theButtonMgr->getBtn("exit_btn")->getSpritePos(), theButtonMgr->getBtn("exit_btn")->getSpriteScale());
		theButtonMgr->getBtn("load_btn")->setSpritePos({ 740, 500 });
		theButtonMgr->getBtn("load_btn")->render(theRenderer, &theButtonMgr->getBtn("load_btn")->getSpriteDimensions(), &theButtonMgr->getBtn("load_btn")->getSpritePos(), theButtonMgr->getBtn("load_btn")->getSpriteScale());
		theButtonMgr->getBtn("save_btn")->setSpritePos({ 740, 575 });
		theButtonMgr->getBtn("save_btn")->render(theRenderer, &theButtonMgr->getBtn("save_btn")->getSpriteDimensions(), &theButtonMgr->getBtn("save_btn")->getSpritePos(), theButtonMgr->getBtn("save_btn")->getSpriteScale());
		

		

		// Render each bullet in the vector array
		for (int draw = 0; draw < theBullets.size(); draw++)
		{
			theBullets[draw]->render(theRenderer, &theBullets[draw]->getSpriteDimensions(), &theBullets[draw]->getSpritePos(), theBullets[draw]->getSpriteRotAngle(), &theBullets[draw]->getSpriteCentre(), theBullets[draw]->getSpriteScale());
		}
		
		//Render the Obstacles
		for (int draw = 0; draw < theAsteroids.size(); draw++)
		{
			
			//cout << "ASTEROID SIZE:"<< theAsteroids.size();
			theAsteroids[draw]->render(theRenderer, &theAsteroids[draw]->getSpriteDimensions(), &theAsteroids[draw]->getSpritePos(), theAsteroids[draw]->getSpriteRotAngle(), &theAsteroids[draw]->getSpriteCentre(), theAsteroids[draw]->getSpriteScale());
		}
	
		//Render the Score
		theTextureMgr->addTexture("ScoreText", theFontMgr->getFont("spaceAge")->createTextTexture(theRenderer, theSScore.c_str(), SOLID, { 0, 255, 0, 255 }, { 0, 0, 0, 0 }));
		cTexture* tempTextTexture2 = theTextureMgr->getTexture("ScoreText");
		SDL_Rect pos2 = { 700, 10, tempTextTexture2->getTextureRect().w, tempTextTexture2->getTextureRect().h };
		FPoint scale2 = { 1, 1 };
		tempTextTexture2->renderTexture(theRenderer, tempTextTexture2->getTexture(), &tempTextTexture2->getTextureRect(), &pos2, scale2);
		theTextureMgr->deleteTexture("ScoreText");
		

		// Render the rocket
		theRocket.render(theRenderer, &theRocket.getSpriteDimensions(), &theRocket.getSpritePos(), theRocket.getSpriteRotAngle(), &theRocket.getSpriteCentre(), theRocket.getSpriteScale());
		SDL_RenderPresent(theRenderer);
	
		


		//cout << resetGame;
		//if (resetGame)
		//{
			//theAsteroids.clear();
			//theBullets.clear();
			//theRocket.setSpritePos({ 500, 550 });
			//theScore == 0;
		//}

		
		

	}
	break;
	case END:
	{
		spriteBkgd.render(theRenderer, NULL, NULL, spriteBkgd.getSpriteScale());
		tempTextTexture = theTextureMgr->getTexture("TitleTxt");
		pos = { 10, 10, tempTextTexture->getTextureRect().w, tempTextTexture->getTextureRect().h };
		tempTextTexture->renderTexture(theRenderer, tempTextTexture->getTexture(), &tempTextTexture->getTextureRect(), &pos, scale);
		tempTextTexture = theTextureMgr->getTexture("ThanksTxt");
		pos = { 300, 10, tempTextTexture->getTextureRect().w, tempTextTexture->getTextureRect().h };
		tempTextTexture->renderTexture(theRenderer, tempTextTexture->getTexture(), &tempTextTexture->getTextureRect(), &pos, scale);
		tempTextTexture = theTextureMgr->getTexture("SeeYouTxt");
		pos = { 300, 75, tempTextTexture->getTextureRect().w, tempTextTexture->getTextureRect().h };
		tempTextTexture->renderTexture(theRenderer, tempTextTexture->getTexture(), &tempTextTexture->getTextureRect(), &pos, scale);
		theButtonMgr->getBtn("menu_btn")->setSpritePos({ 500, 500 });
		theButtonMgr->getBtn("menu_btn")->render(theRenderer, &theButtonMgr->getBtn("menu_btn")->getSpriteDimensions(), &theButtonMgr->getBtn("menu_btn")->getSpritePos(), theButtonMgr->getBtn("menu_btn")->getSpriteScale());
		theButtonMgr->getBtn("exit_btn")->setSpritePos({ 500, 575 });
		theButtonMgr->getBtn("exit_btn")->render(theRenderer, &theButtonMgr->getBtn("exit_btn")->getSpriteDimensions(), &theButtonMgr->getBtn("exit_btn")->getSpritePos(), theButtonMgr->getBtn("exit_btn")->getSpriteScale());
		//resetGame = false;
		
			
	
	}
	break;
	
	case QUIT:
	{
		loop = false;
	}
	break;

	default:
	break;
	}

	SDL_RenderPresent(theRenderer);   
	
	


}

void cGame::render(SDL_Window* theSDLWND, SDL_Renderer* theRenderer, double rotAngle, SDL_Point* spriteCentre)
{

	SDL_RenderPresent(theRenderer);
}

void cGame::update()
{

}

void cGame::update(double deltaTime)
{

	if (theGameState == MENU || theGameState == END)
	{
		//if (theButtonMgr->getBtn("exit_btn")->getClicked())
		//{
		//	resetGame = false; //Reset the end check
		//	theButtonMgr->getBtn("exit_btn")->setClicked(false); //Reset the exit button
		//}
		theGameState = theButtonMgr->getBtn("exit_btn")->update(theGameState, QUIT, theAreaClicked);
	}
	else
	{
		//if (theButtonMgr->getBtn("exit_btn")->getClicked())
		//{
		//	resetGame = true; //Reset the end check
		//	theButtonMgr->getBtn("exit_btn")->setClicked(false); //Reset the exit button
		//}
		theGameState = theButtonMgr->getBtn("exit_btn")->update(theGameState, END, theAreaClicked);
	}
	theGameState = theButtonMgr->getBtn("play_btn")->update(theGameState, PLAYING, theAreaClicked);
	theGameState = theButtonMgr->getBtn("menu_btn")->update(theGameState, MENU, theAreaClicked);

	if (theGameState == MENU || theGameState == END)
	{
		if (theButtonMgr->getBtn("play_btn")->getClicked())
		{
			theAsteroids.clear();
			theRocket.setSpritePos({ 500, 550 });
			theScore -= theScore;
			//theTextureMgr->deleteTexture("ScoreText");
			


			for (int counter = 0; counter < 3; counter++)
			{
				theAsteroids.push_back(new cAsteroid);
				theAsteroids[counter]->setSpritePos({ 220 * (1 + counter), 0 });
				//theAsteroids[counter]->setSpriteTranslation({ 0, 8 });

				randAsteroid = rand() % 3;
				//cout << counter << ": " << randAsteroid << "   ";
				laneSpeed[counter] = rand() % 8 + 1;
				theAsteroids[counter]->setTexture(theTextureMgr->getTexture(textureName[randAsteroid]));
				theAsteroids[counter]->setSpriteDimensions(theTextureMgr->getTexture(textureName[randAsteroid])->getTWidth(), theTextureMgr->getTexture(textureName[randAsteroid])->getTHeight());
				//	theAsteroids[counter]->setAsteroidVelocity({ 3.0f, 3.0f });
				theAsteroids[counter]->setActive(true);
				//theAsteroids[counter]->setSpritePos({ theAsteroids[counter]->getSpritePos().x, theAsteroids[counter]->getSpritePos().y + laneSpeed[counter] });

			}

			

		}

		


	}


	switch (theGameState)
	{
	case MENU:
	{
		//theGameState = theButtonMgr->getBtn("exit_btn")->update(theGameState, QUIT, theAreaClicked);
		//theGameState = theButtonMgr->getBtn("play_btn")->update(theGameState, PLAYING, theAreaClicked); 
	}
	break;
	case PLAYING:
	{
		
		vector<cAsteroid*>::iterator asteroidIterator = theAsteroids.begin();
		while (asteroidIterator != theAsteroids.end())
		{
			if ((*asteroidIterator)->isActive() == false)
			{
				asteroidIterator = theAsteroids.erase(asteroidIterator);
			}
			else
			{
				(*asteroidIterator)->update(deltaTime);
				++asteroidIterator;
			}
		}


		// Update the visibility and position of each bullet
		vector<cBullet*>::iterator bulletIterartor = theBullets.begin();
		while (bulletIterartor != theBullets.end())
		{
			if ((*bulletIterartor)->isActive() == false)
			{
				bulletIterartor = theBullets.erase(bulletIterartor);
			}
			else
			{
				(*bulletIterartor)->update(deltaTime);
				++bulletIterartor;
			}
		}
		/*
		==============================================================
		| Check for collisions
		==============================================================
		*/
		for (vector<cBullet*>::iterator bulletIterartor = theBullets.begin(); bulletIterartor != theBullets.end(); ++bulletIterartor)
		{
			//(*bulletIterartor)->update(deltaTime);
			for (vector<cAsteroid*>::iterator asteroidIterator = theAsteroids.begin(); asteroidIterator != theAsteroids.end(); ++asteroidIterator)
			{
				if ((*asteroidIterator)->collidedWith(&(*asteroidIterator)->getBoundingRect(), &(*bulletIterartor)->getBoundingRect()))
				{
					// if a collision set the bullet and asteroid to false
					(*asteroidIterator)->setActive(false);

					//asteroidShot = theAsteroids[].at(asteroidIterator)= asteroidIterator;
					//asteroidShot = *asteroidIterator;
					(*bulletIterartor)->setActive(false);
					theSoundMgr->getSnd("explosion")->play(0);
					theScore++;
					theSScore = gameTextList[3] + to_string(theScore);
					//theTextureMgr->deleteTexture("ScoreText");
					asteroidShot = collisionNum;
					if (collisionNum < 2)
					{
						collisionNum++;
					}
					else
					{
						collisionNum = 0;
					}

				}
				else
				{
					if (collisionNum < 2)
					{
						collisionNum++;
						cout << "CN HAS INCREASED";
					}
					else
					{
						collisionNum = 0;
						cout << "CN HAS RESET";
					}
				}

			}
		}
		// THis is a change
		cout << "Ahhhhhhhhhhh!!!!!!!!!!";
		for (vector<cAsteroid*>::iterator asteroidIterator = theAsteroids.begin(); asteroidIterator != theAsteroids.end(); ++asteroidIterator)
		{
			if ((*asteroidIterator)->collidedWith(&(*asteroidIterator)->getBoundingRect(), &(theRocket.getBoundingRect())))
			{
				//resetGame = true;
				cout << "Aster Rect: " << (*asteroidIterator)->getBoundingRect().x << "," << (*asteroidIterator)->getBoundingRect().y << "," << (*asteroidIterator)->getBoundingRect().w << "," << (*asteroidIterator)->getBoundingRect().h;
				//cout << "asIt: " << *asteroidIterator;
				cout << "Rocket Rect: " << theRocket.getBoundingRect().x << "," << theRocket.getBoundingRect().y << "," << theRocket.getBoundingRect().w << "," << theRocket.getBoundingRect().h;
				theGameState = END;
				//system("pause");
			}
		}
		
		
		
		
		//theGameState = theButtonMgr->getBtn("exit_btn")->update(theGameState, END, theAreaClicked);
		//theGameState = theButtonMgr->getBtn("load_btn")->update(theGameState, LOADMAP, theAreaClicked);



		//for (int col = 0; col < theAsteroids.size(); col++)
		//{
		//	theAsteroids.push_back(new cAsteroid);
		//	if ((theAsteroids[col])->collidedWith(&(theAsteroids[col])->getBoundingRect(), &(theRocket.getBoundingRect)()));;
		//	{
		//		//cout << "col: " << col;
		//		//theGameState = END;
		//	}
		//
		//}
	
	}
	break;
	case END:
	{
		if (theButtonMgr->getBtn("exit_btn")->getClicked())
		{
		//	theTextureMgr->deleteTexture("ScoreTexture");
		}
		
		//theGameState = theButtonMgr->getBtn("exit_btn")->update(theGameState, QUIT, theAreaClicked);
		//theGameState = theButtonMgr->getBtn("load_btn")->update(theGameState, PLAYING, theAreaClicked); 
	}
	break;


	default:
		break;


	}




	// Update the visibility and position of each asteriod
	/*vector<cAsteroid*>::iterator asteroidIterator = theAsteroids.begin();
	while (asteroidIterator != theAsteroids.end())
	{
		if ((*asteroidIterator)->isActive() == false)
		{
			asteroidIterator = theAsteroids.erase(asteroidIterator);
		}
		else
		{
			(*asteroidIterator)->update(deltaTime);
			++asteroidIterator;
		}
	}*/


	/*for (int counter = 0; counter < 3; counter++)
	{
		theAsteroids.push_back(new cAsteroid);
		theAsteroids[counter]->setSpritePos({ theAsteroids[counter]->getSpritePos().x, theAsteroids[counter]->getSpritePos().y + laneSpeed[counter] });
	}*/
	
	
	//for (int counter = 0; counter < 3; counter++)
	//{
	//	theAsteroids.push_back(new cAsteroid);
	//	//theAsteroids[counter]->setSpritePos({ 240 * (1 + counter), 25 });
	//	theAsteroids[counter]->setSpriteTranslation({ 0, 8 });
	//	int randAsteroid = rand() % 3;
	//	laneSpeed[counter] = rand() % 14 + 1;
	//	theAsteroids[counter]->setTexture(theTextureMgr->getTexture(textureName[randAsteroid]));
	//	theAsteroids[counter]->setSpriteDimensions(theTextureMgr->getTexture(textureName[randAsteroid])->getTWidth(), theTextureMgr->getTexture(textureName[randAsteroid])->getTHeight());
	//	theAsteroids[counter]->setAsteroidVelocity({ 3.0f, 3.0f });
	//	theAsteroids[counter]->setActive(true);
	//	theAsteroids[counter]->setSpritePos({ theAsteroids[counter]->getSpritePos().x, theAsteroids[counter]->getSpritePos().y + laneSpeed[counter] });
	//} 


	//for (int counter = 0; counter < theAsteroids.size(); counter++)
	//{
	//	if (theAsteroids[counter]->getSpritePos().y > 768);
	//	laneClear = true;
	//}

	//if (laneClear == true)
	//{
	//	//randomly pick a lane 
	//	int newSpawnLane = rand() % 3;
	//	int newSpriteLoad = rand() % 3;
	//	numOfAsteroids++;
	//    //pick a random sprite
	//	theAsteroids[numOfAsteroids]->setTexture(theTextureMgr->getTexture(textureName[newSpriteLoad]));
	//    theAsteroids[numOfAsteroids]->setSpriteDimensions(theTextureMgr->getTexture(textureName[numOfAsteroids])->getTWidth(), theTextureMgr->getTexture(textureName[numOfAsteroids])->getTHeight());
	//	// make the sprite in one of the lanes
	//	laneSpeed[numOfAsteroids] = rand() % 14 + 1;
	//	theAsteroids[numOfAsteroids]->setSpritePos({ theAsteroids[numOfAsteroids]->getSpritePos().x, theAsteroids[numOfAsteroids]->getSpritePos().y + laneSpeed[numOfAsteroids] });
	//	// reset the lane clear bool
	//	laneClear = false;
	//}

		//theAsteroids.push_back(new cAsteroid);
		//theAsteroid[1].setSpritePos({ theAsteroid.getSpritePos.x(), theAsteroid.getSpritePos.y() + 5 });
		//theAsteroid[counter].setSpritePos({ theAsteroid.getSpritePos().x, theAsteroid.getSpritePos().y + 5 });


	//// Update the visibility and position of each bullet
	//vector<cBullet*>::iterator bulletIterartor = theBullets.begin();
	//while (bulletIterartor != theBullets.end())
	//{
	//	if ((*bulletIterartor)->isActive() == false)
	//	{
	//		bulletIterartor = theBullets.erase(bulletIterartor);
	//	}
	//	else
	//	{
	//		(*bulletIterartor)->update(deltaTime);
	//		++bulletIterartor;
	//	}
	//}
	///*
	//==============================================================
	//| Check for collisions
	//==============================================================
	//*/
	//for (vector<cBullet*>::iterator bulletIterartor = theBullets.begin(); bulletIterartor != theBullets.end(); ++bulletIterartor)
	//{
	//	//(*bulletIterartor)->update(deltaTime);
 //		for (vector<cAsteroid*>::iterator asteroidIterator = theAsteroids.begin(); asteroidIterator != theAsteroids.end(); ++asteroidIterator)
	//	{  
	//		if ((*asteroidIterator)->collidedWith(&(*asteroidIterator)->getBoundingRect(), &(*bulletIterartor)->getBoundingRect()))
	//		{
	//			// if a collision set the bullet and asteroid to false
	//			(*asteroidIterator)->setActive(false);
	//			
	//			//asteroidShot = theAsteroids[].at(asteroidIterator)= asteroidIterator;
	//			//asteroidShot = *asteroidIterator;
	//			(*bulletIterartor)->setActive(false);
	//			theSoundMgr->getSnd("explosion")->play(0); 
	//			theScore++;
	//			theSScore = gameTextList[3] + to_string(theScore);
	//			//theTextureMgr->deleteTexture("ScoreText");
	//			asteroidShot = collisionNum;
	//			if (collisionNum < 2)
	//			{
	//				collisionNum++;
	//			}
	//			else
	//			{
	//				collisionNum = 0;
	//			}
	//			
	//		}
	//		else
	//		{
	//			if (collisionNum < 2)
	//			{
	//				collisionNum++;
	//				cout << "CN HAS INCREASED";
	//			}
	//			else
	//			{
	//				collisionNum = 0;
	//				cout << "CN HAS RESET";
	//			}
	//		}

	//	}
	//}


	

	//for (int col = 0; col < theAsteroids.size(); col++)
	//{
	//	theAsteroids.push_back(new cAsteroid);
	//	if ((theAsteroids[col])->collidedWith(&(theAsteroids[col])->getBoundingRect(), &(theRocket.getBoundingRect)()));;
	//	{
	//		//theGameState = END;
	//	}
	//
	//}


	// Update the Rockets position
	theRocket.update(deltaTime);
}

bool cGame::getInput(bool theLoop)
{
	SDL_Event event;

	while (SDL_PollEvent(&event))
	{
		if (event.type == SDL_QUIT)
		{
			theLoop = false;
		}

		switch (event.type)
		{
			case SDL_MOUSEBUTTONDOWN:
				switch (event.button.button)
				{
				case SDL_BUTTON_LEFT:
				{
					theAreaClicked = { event.motion.x, event.motion.y};
				}
				break;
				case SDL_BUTTON_RIGHT:
					break;
				default:
					break;
				}
				break;
			case SDL_MOUSEBUTTONUP:
				switch (event.button.button)
				{
				case SDL_BUTTON_LEFT:
				{
				}
				break;
				case SDL_BUTTON_RIGHT:
					break;
				default:
					break;
				}
				break;
			case SDL_MOUSEMOTION:
			break;
			case SDL_KEYDOWN:
				switch (event.key.keysym.sym)
				{
				case SDLK_ESCAPE:
					theLoop = false;
					break;
				case SDLK_DOWN:
				{
					//if (theRocket.getSpritePos().x < (renderWidth - theRocket.getSpritePos().w))
					//{
					//	theRocket.setSpriteTranslation({ -5, -5 });
				//	}
				}
				break;
			
				case SDLK_UP:
				{
					//if (theRocket.getSpritePos().x > 0)
					//{
						//theRocket.setSpriteTranslation({ 5, 5 });
					//}
				}
				break;
				case SDLK_d:
				{
					if (theRocket.getSpritePos().x < 850)
					{
						theRocket.setSpritePos({theRocket.getSpritePos().x +12 , theRocket.getSpritePos().y});
					}
				}
				break;
				//if (theRocket.getSpritePos().x >= 0 && theRocket.getSpritePos().x <= 700)
				case SDLK_a:
				{
					if (theRocket.getSpritePos().x > 0)
					{
						theRocket.setSpritePos({ theRocket.getSpritePos().x - 12, theRocket.getSpritePos().y });

					}
				}
				break;
				case SDLK_SPACE:
				{
					theBullets.push_back(new cBullet);
					int numBullets = theBullets.size() - 1;
					theBullets[numBullets]->setSpriteScale({ 1.0f, 0.25f });
					theBullets[numBullets]->setSpritePos({ theRocket.getBoundingRect().x + theRocket.getSpriteCentre().x, theRocket.getBoundingRect().y + theRocket.getSpriteCentre().y });
					theBullets[numBullets]->setSpriteTranslation({ 2.0f, 2.0f });
					theBullets[numBullets]->setTexture(theTextureMgr->getTexture("grenade"));
					theBullets[numBullets]->setSpriteDimensions(theTextureMgr->getTexture("grenade")->getTWidth(), theTextureMgr->getTexture("grenade")->getTHeight());
					theBullets[numBullets]->setBulletVelocity({ 2.0f, 2.0f });
					theBullets[numBullets]->setSpriteRotAngle(theRocket.getSpriteRotAngle());
					theBullets[numBullets]->setActive(true);
					cout << "Bullet added to Vector at position - x: " << theRocket.getBoundingRect().x << " y: " << theRocket.getBoundingRect().y << endl;
				}
				theSoundMgr->getSnd("shot")->play(0);
				break;
				default:
					break;
				}

			default:
				break;
		}

	}
	return theLoop;
}

double cGame::getElapsedSeconds()
{
	this->m_CurrentTime = high_resolution_clock::now();
	this->deltaTime = (this->m_CurrentTime - this->m_lastTime);
	this->m_lastTime = this->m_CurrentTime;
	return deltaTime.count();
}

void cGame::cleanUp(SDL_Window* theSDLWND)
{
	// Delete our OpengL context
	SDL_GL_DeleteContext(theSDLWND);

	// Destroy the window
	SDL_DestroyWindow(theSDLWND);

	// Quit IMG system
	IMG_Quit();

	// Shutdown SDL 2
	SDL_Quit();
}



