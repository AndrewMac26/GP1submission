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
	//Initialise Function is called at the start of the game execution
	// and is used for setup purposes before the game itself is run.

	// Get width and height of render context
	SDL_GetRendererOutputSize(theRenderer, &renderWidth, &renderHeight);
	this->m_lastTime = high_resolution_clock::now();
	// Clear the buffer with a black background
	SDL_SetRenderDrawColor(theRenderer, 0, 0, 0, 255);
	SDL_RenderPresent(theRenderer);
	theScore = 0;

	theTextureMgr->setRenderer(theRenderer); //Calls the Renderer
	theFontMgr->initFontLib(); //Calls the font manager
	theSoundMgr->initMixer(); //calls the sound manager
	theAreaClicked = { 0, 0 }; //Clears area clicked
	
	theTextureMgr->setRenderer(theRenderer);
	theFontMgr->initFontLib();
	theSoundMgr->initMixer();

	// Store the textures
	textureName = { "ringOfFire","sharkTank","ramp","asteroid1", "asteroid2", "asteroid3", "asteroid4", "bullet","theRocket","theBackground", "grenade", "roadBackground" ,"playerCharacter", "menuBackground", "loseBackground"};
	texturesToUse = { "Images\\ringOfFire.png","Images\\sharkTank.png","Images\\ramp.png","Images\\asteroid1.png", "Images\\asteroid2.png", "Images\\asteroid3.png", "Images\\asteroid4.png", "Images\\bullet.png", "Images\\rocketSprite.png", "Images\\starscape1024x768.png", "Images\\grenade.png", "Images\\mainSceneBackground.png", "Images\\car.png", "Images\\menuBackgound.png", "Images\\loseBackground.png" };
	//Assigns "texturesToUse" to "textureName" based on their postions"
	for (int tCount = 0; tCount < textureName.size(); tCount++)
	{	
		theTextureMgr->addTexture(textureName[tCount], texturesToUse[tCount]);
	}
	//Stores buttons
	btnNameList = { "exit_btn", "instructions_btn", "load_btn", "menu_btn", "play_btn", "save_btn", "settings_btn" }; //Array of button name list
	btnTexturesToUse = { "Images/Buttons/button_exit.png", "Images/Buttons/button_instructions.png", "Images/Buttons/button_load.png", "Images/Buttons/button_menu.png", "Images/Buttons/button_play.png", "Images/Buttons/button_save.png", "Images/Buttons/button_settings.png" }; //Button texture file locations
	btnPos = { { 350, 375 }, { 350, 300 }, { 400, 300 }, { 325, 500 }, { 400, 300 }, { 740, 500 }, { 400, 300 } }; //Position of the buttons
	//Assigns "btnTexturesToUse" to "btnNameList" based on their postions
	for (int bCount = 0; bCount < btnNameList.size(); bCount++) //For integar bcount equals 0, bcount is less than the button name list, add 1 to bcount
	{
		theTextureMgr->addTexture(btnNameList[bCount], btnTexturesToUse[bCount]); //Adds the button texture to use
		cButton * newBtn = new cButton(); //newBtn equals to new cButton()
		newBtn->setTexture(theTextureMgr->getTexture(btnNameList[bCount])); //Sets texture for the button
		newBtn->setSpritePos(btnPos[bCount]); //Sets button position
		newBtn->setSpriteDimensions(theTextureMgr->getTexture(btnNameList[bCount])->getTWidth(), theTextureMgr->getTexture(btnNameList[bCount])->getTHeight()); //Sets button dimensions
		theButtonMgr->add(btnNameList[bCount], newBtn); //Adds button
	}


	theGameState = MENU; //Set the game state to menu
	theBtnType = EXIT; //Sets button type to exit


	// Stores the fonts 
	fontList = { "digital", "spaceAge", "quantum" };
	fontsToUse = { "Fonts/digital-7.ttf", "Fonts/space age.ttf", "Fonts/SF Laundromatic.ttf" };
	//Asigns the font names to the file locations
	for (int fonts = 0; fonts < fontList.size(); fonts++)
	{
		theFontMgr->addFont(fontList[fonts], fontsToUse[fonts], 36);
	}
	

	// Create text Textures
	gameTextNames = { "TitleTxt", "ThanksTxt", "SeeYouTxt", "ScoreText", "MenuText", "MenuExp"};
	gameTextList = { "All My Own Stunts", "Bad Luck!", "Why not play again?", "Score: ", "Welcome! Have Fun!", "Try to avoid doing any stunts by shooting them!" };

	//Assigns Text Textures to their contents 
	for (int text = 0; text < gameTextNames.size(); text++) 
	{
		theTextureMgr->addTexture(gameTextNames[text], theFontMgr->getFont("quantum")->createTextTexture(theRenderer, gameTextList[text], SOLID, { 0, 0, 0, 255 }, { 0, 0, 0, 0 })); //Adds the font type digital and have a solid clear background to it
	}
	// Loads game sounds
	soundList = { "theme", "shot", "explosion"};
	soundTypes = { MUSIC, SFX, SFX };
	soundsToUse = { "Audio/youBetterCallKennyLoggins.wav", "Audio/gunshot.wav", "Audio/FSExplosion.mp3"};
	//Assigns sound file path to the sound names
	for (int sounds = 0; sounds < soundList.size(); sounds++)
	{
		theSoundMgr->add(soundList[sounds], soundsToUse[sounds], soundTypes[sounds]);
	}
	//Creates background by making the "theme sound clip play on loop"
	theSoundMgr->getSnd("theme")->play(-1); // -1 makes it loop

	//Initalises the background for the main game screen 
	spriteBkgd.setSpritePos({ 0, 0 });
	spriteBkgd.setTexture(theTextureMgr->getTexture("roadBackground"));
	spriteBkgd.setSpriteDimensions(theTextureMgr->getTexture("roadBackground")->getTWidth(), theTextureMgr->getTexture("roadBackground")->getTHeight());

	//Initialises the position and texture of the players sprite
	theRocket.setSpritePos({ 500, 550 });
	theRocket.setTexture(theTextureMgr->getTexture("playerCharacter"));
	theRocket.setSpriteDimensions(theTextureMgr->getTexture("playerCharacter")->getTWidth(), theTextureMgr->getTexture("playerCharacter")->getTHeight());
	theRocket.setRocketVelocity({ 0, 0 });

	 //Creates an initial vector array of textures
	for (int counter = 0; counter < 3; counter++)
	{
		theAsteroids.push_back(new cAsteroid);
		theAsteroids[counter]->setSpritePos({ 220 * (1 + counter), 0 });
		randAsteroid = rand() %  3;
		laneSpeed[counter] = rand() % 8 + 1;
		theAsteroids[counter]->setTexture(theTextureMgr->getTexture(textureName[randAsteroid]));
		theAsteroids[counter]->setSpriteDimensions(theTextureMgr->getTexture(textureName[randAsteroid])->getTWidth(), theTextureMgr->getTexture(textureName[randAsteroid])->getTHeight());
		theAsteroids[counter]->setActive(true);
	}
	
	//Temporary variable for storing scores
	theSScore = gameTextList[3] + to_string(theScore);

}

void cGame::run(SDL_Window* theSDLWND, SDL_Renderer* theRenderer)
{
	//run loop which is used to continue or terminate the programs execution
	loop = true;

	while (loop)
	{
		//Get the time that passed since the last frame
		double elapsedTime = this->getElapsedSeconds();

		loop = this->getInput(loop);
		this->update(elapsedTime);
		this->render(theSDLWND, theRenderer);
	}
}

void cGame::render(SDL_Window* theSDLWND, SDL_Renderer* theRenderer)
{ 
	
	SDL_RenderClear(theRenderer); //Clears the render
	//case statement with 3 cases for MENU, PLAYING and END
	switch (theGameState)
	{
	//Start of Menu Case (Renderer)
	case MENU:
	{
		//Renders the menu background
		spriteBkgd.setSpritePos({ 0, 0 });
		spriteBkgd.setTexture(theTextureMgr->getTexture("menuBackground"));
		spriteBkgd.setSpriteDimensions(theTextureMgr->getTexture("menuBackground")->getTWidth(), theTextureMgr->getTexture("menuBackground")->getTHeight());
		spriteBkgd.render(theRenderer, NULL, NULL, spriteBkgd.getSpriteScale());
		// Renders text on the menu screen 
		tempTextTexture = theTextureMgr->getTexture("TitleTxt"); 
		pos = { 350, 10, tempTextTexture->getTextureRect().w, tempTextTexture->getTextureRect().h }; 
		scale = { 1, 1 }; 
		tempTextTexture->renderTexture(theRenderer, tempTextTexture->getTexture(), &tempTextTexture->getTextureRect(), &pos, scale); 
		tempTextTexture = theTextureMgr->getTexture("MenuText");
		pos = { 250, 120, tempTextTexture->getTextureRect().w, tempTextTexture->getTextureRect().h }; 
		tempTextTexture->renderTexture(theRenderer, tempTextTexture->getTexture(), &tempTextTexture->getTextureRect(), &pos, scale); 
		tempTextTexture = theTextureMgr->getTexture("MenuExp"); 
		pos = { 200, 250, tempTextTexture->getTextureRect().w, tempTextTexture->getTextureRect().h }; 
		tempTextTexture->renderTexture(theRenderer, tempTextTexture->getTexture(), &tempTextTexture->getTextureRect(), &pos, scale);

		//Rendering Buttons for main menu
		theButtonMgr->getBtn("play_btn")->render(theRenderer, &theButtonMgr->getBtn("play_btn")->getSpriteDimensions(), &theButtonMgr->getBtn("play_btn")->getSpritePos(), theButtonMgr->getBtn("play_btn")->getSpriteScale()); 
		theButtonMgr->getBtn("exit_btn")->setSpritePos({ 350, 375 }); 
		theButtonMgr->getBtn("exit_btn")->render(theRenderer, &theButtonMgr->getBtn("exit_btn")->getSpriteDimensions(), &theButtonMgr->getBtn("exit_btn")->getSpritePos(), theButtonMgr->getBtn("exit_btn")->getSpriteScale()); 
	}
	break;
	//End of menu case(Renderer)
	// Start of Playing case (Renderer)
	case PLAYING:
	{
		
		//Loop to ensure that theAsteroids is always full of obstacles and that there are always 3 obstacles spawned in when the game is actually playing
		for (int spawnCount = 0; spawnCount < theAsteroids.size(); spawnCount++)
		{
			if (theAsteroids[spawnCount]->getSpritePos().y > 760 || spawnCount == asteroidShot) //asteroidShot found below, it dertermines which, if any of the obstacle have been shot so they can be replaced
			{
				theAsteroids[spawnCount]->setSpritePos({ 220 * (1 + spawnCount), 0 }); // spawns at intervals to create a "lanes" effect (3 lanes)
						randObs[spawnCount] = rand() % 3;    //finds a random number to decide which of the 3 obstacles are spawned 
						int currentObs = randObs[spawnCount]; // randObs is assigned to this int (temp var) so that it can be passed into textureName
						theAsteroids[spawnCount]->setTexture(theTextureMgr->getTexture(textureName[currentObs])); // assigns a texture based on the random number generated
						theAsteroids[spawnCount]->setSpriteDimensions(theTextureMgr->getTexture(textureName[currentObs])->getTWidth(), theTextureMgr->getTexture(textureName[currentObs])->getTHeight()); // fetches sprite dimesions
						laneSpeed[spawnCount] = rand() % 5 + 2; // RNG to determine the speed the obstacle moves across the screen
						theAsteroids[spawnCount]->setActive(true); // sets all sprites generated to active 
						currentObs = 0; //resets currentObs temp variable to zero for next loop
						asteroidShot = 4; //sets asteroidShot to 4 to ensure loop runs correctly 
			}
			theAsteroids[spawnCount]->setSpritePos({ theAsteroids[spawnCount]->getSpritePos().x, theAsteroids[spawnCount]->getSpritePos().y + laneSpeed[spawnCount] }); // applys predetermined movement to sprites 
		}

		//Renders main game background 
		spriteBkgd.setSpritePos({ 0, 0 });
		spriteBkgd.setTexture(theTextureMgr->getTexture("roadBackground"));
		spriteBkgd.setSpriteDimensions(theTextureMgr->getTexture("roadBackground")->getTWidth(), theTextureMgr->getTexture("roadBackground")->getTHeight());
		spriteBkgd.render(theRenderer, NULL, NULL, spriteBkgd.getSpriteScale());
		// Renders title in the main game 
		tempTextTexture = theTextureMgr->getTexture("TitleTxt");
		pos = { 10, 10, tempTextTexture->getTextureRect().w, tempTextTexture->getTextureRect().h };
		tempTextTexture->renderTexture(theRenderer, tempTextTexture->getTexture(), &tempTextTexture->getTextureRect(), &pos, scale);
		//Renders buttons for Playing scene
		theButtonMgr->getBtn("exit_btn")->setSpritePos({ 910, 650 });
		theButtonMgr->getBtn("exit_btn")->render(theRenderer, &theButtonMgr->getBtn("exit_btn")->getSpriteDimensions(), &theButtonMgr->getBtn("exit_btn")->getSpritePos(), theButtonMgr->getBtn("exit_btn")->getSpriteScale());
		theButtonMgr->getBtn("load_btn")->setSpritePos({ 910, 500 });
		theButtonMgr->getBtn("load_btn")->render(theRenderer, &theButtonMgr->getBtn("load_btn")->getSpriteDimensions(), &theButtonMgr->getBtn("load_btn")->getSpritePos(), theButtonMgr->getBtn("load_btn")->getSpriteScale());
		theButtonMgr->getBtn("save_btn")->setSpritePos({ 910, 575 });
		theButtonMgr->getBtn("save_btn")->render(theRenderer, &theButtonMgr->getBtn("save_btn")->getSpriteDimensions(), &theButtonMgr->getBtn("save_btn")->getSpritePos(), theButtonMgr->getBtn("save_btn")->getSpriteScale());
		

		

		// Render each bullet in the vector array
		for (int draw = 0; draw < theBullets.size(); draw++)
		{
			theBullets[draw]->render(theRenderer, &theBullets[draw]->getSpriteDimensions(), &theBullets[draw]->getSpritePos(), theBullets[draw]->getSpriteRotAngle(), &theBullets[draw]->getSpriteCentre(), theBullets[draw]->getSpriteScale());
		}
		
		//Render the Obstacles
		for (int draw = 0; draw < theAsteroids.size(); draw++)
		{
			theAsteroids[draw]->render(theRenderer, &theAsteroids[draw]->getSpriteDimensions(), &theAsteroids[draw]->getSpritePos(), theAsteroids[draw]->getSpriteRotAngle(), &theAsteroids[draw]->getSpriteCentre(), theAsteroids[draw]->getSpriteScale());
		}
	
		//Render the Score
		theTextureMgr->addTexture("ScoreText", theFontMgr->getFont("quantum")->createTextTexture(theRenderer, theSScore.c_str(), SOLID, { 0, 255, 0, 255 }, { 0, 0, 0, 0 }));
		cTexture* tempTextTexture2 = theTextureMgr->getTexture("ScoreText");
		SDL_Rect pos2 = { 700, 10, tempTextTexture2->getTextureRect().w, tempTextTexture2->getTextureRect().h };
		FPoint scale2 = { 1, 1 };
		tempTextTexture2->renderTexture(theRenderer, tempTextTexture2->getTexture(), &tempTextTexture2->getTextureRect(), &pos2, scale2);
		theTextureMgr->deleteTexture("ScoreText");
		

		// Render the rocket
		theRocket.render(theRenderer, &theRocket.getSpriteDimensions(), &theRocket.getSpritePos(), theRocket.getSpriteRotAngle(), &theRocket.getSpriteCentre(), theRocket.getSpriteScale());
		SDL_RenderPresent(theRenderer);
	
	}
	break;
	//End of playing case (Renderer)
	//Start of end case (Render)
	case END:
	{
		theAreaClicked = { 0, 0 }; // clears area clicked to avid issues with buttons in the same place but on difrent scenes 
		//Renders the END scene background 
		spriteBkgd.setSpritePos({ 0, 0 });
		spriteBkgd.setTexture(theTextureMgr->getTexture("loseBackground"));
		spriteBkgd.setSpriteDimensions(theTextureMgr->getTexture("loseBackground")->getTWidth(), theTextureMgr->getTexture("loseBackground")->getTHeight());
		spriteBkgd.render(theRenderer, NULL, NULL, spriteBkgd.getSpriteScale());
		// Renders text 
		tempTextTexture = theTextureMgr->getTexture("TitleTxt");
		pos = { 10, 10, tempTextTexture->getTextureRect().w, tempTextTexture->getTextureRect().h };
		tempTextTexture->renderTexture(theRenderer, tempTextTexture->getTexture(), &tempTextTexture->getTextureRect(), &pos, scale);
		tempTextTexture = theTextureMgr->getTexture("ThanksTxt");
		pos = { 300, 10, tempTextTexture->getTextureRect().w, tempTextTexture->getTextureRect().h };
		tempTextTexture->renderTexture(theRenderer, tempTextTexture->getTexture(), &tempTextTexture->getTextureRect(), &pos, scale);
		tempTextTexture = theTextureMgr->getTexture("SeeYouTxt");
		pos = { 300, 75, tempTextTexture->getTextureRect().w, tempTextTexture->getTextureRect().h };
		tempTextTexture->renderTexture(theRenderer, tempTextTexture->getTexture(), &tempTextTexture->getTextureRect(), &pos, scale);
		//Renders buttons for the end scene 
		theButtonMgr->getBtn("menu_btn")->setSpritePos({ 500, 500 });
		theButtonMgr->getBtn("menu_btn")->render(theRenderer, &theButtonMgr->getBtn("menu_btn")->getSpriteDimensions(), &theButtonMgr->getBtn("menu_btn")->getSpritePos(), theButtonMgr->getBtn("menu_btn")->getSpriteScale());
		theButtonMgr->getBtn("exit_btn")->setSpritePos({ 500, 575 });
		theButtonMgr->getBtn("exit_btn")->render(theRenderer, &theButtonMgr->getBtn("exit_btn")->getSpriteDimensions(), &theButtonMgr->getBtn("exit_btn")->getSpritePos(), theButtonMgr->getBtn("exit_btn")->getSpriteScale());
			
	
	}
	break;
	//End of END case (Renderer)
	//Case which holds the boolean used to control if the game is running
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
	//Update function used to check collison etc

	//Assigns button functions
	if (theGameState == MENU || theGameState == END)
	{
		theGameState = theButtonMgr->getBtn("exit_btn")->update(theGameState, QUIT, theAreaClicked);
	}
	else
	{
		theGameState = theButtonMgr->getBtn("exit_btn")->update(theGameState, END, theAreaClicked);
	}
	theGameState = theButtonMgr->getBtn("play_btn")->update(theGameState, PLAYING, theAreaClicked);
	theGameState = theButtonMgr->getBtn("menu_btn")->update(theGameState, MENU, theAreaClicked);

	if (theGameState == MENU || theGameState == END)
	{
		//Used to reset all relevant varibales to ensure the reset feature works as intended 
		if (theButtonMgr->getBtn("play_btn")->getClicked())
		{
			theAsteroids.clear();
			theRocket.setSpritePos({ 500, 550 });
			theScore -= theScore;
			theAreaClicked = { 0, 0 };
			collisionNum = 0;
			
			

			//Reintantiates obsactle array(also found in initalise)
			for (int counter = 0; counter < 3; counter++)
			{
				theAsteroids.push_back(new cAsteroid);
				theAsteroids[counter]->setSpritePos({ 220 * (1 + counter), 0 });
				randAsteroid = rand() % 3;
				laneSpeed[counter] = rand() % 8 + 1;
				theAsteroids[counter]->setTexture(theTextureMgr->getTexture(textureName[randAsteroid]));
				theAsteroids[counter]->setSpriteDimensions(theTextureMgr->getTexture(textureName[randAsteroid])->getTWidth(), theTextureMgr->getTexture(textureName[randAsteroid])->getTHeight());
				theAsteroids[counter]->setActive(true);
			}
		}
	}


	switch (theGameState)
	{
	case MENU:
	{
		
	}
	break;
	case PLAYING:
	{
		//Updates postion of the obstacles
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
		//Loops once through the bullets vector array
		for (vector<cBullet*>::iterator bulletIterartor = theBullets.begin(); bulletIterartor != theBullets.end(); ++bulletIterartor)
		{
			//Loops once throught the obstacle array (theAsteroids)
			for (vector<cAsteroid*>::iterator asteroidIterator = theAsteroids.begin(); asteroidIterator != theAsteroids.end(); ++asteroidIterator)
			{
				if ((*asteroidIterator)->collidedWith(&(*asteroidIterator)->getBoundingRect(), &(*bulletIterartor)->getBoundingRect()))
				{
					//if there is a collision
					(*asteroidIterator)->setActive(false); // deactivate asteroid involved in collision 
					(*bulletIterartor)->setActive(false);  // deactivate bullet involved in collision 
					theSoundMgr->getSnd("explosion")->play(0); // plays explosion sound
					theScore++; // increments score
					theSScore = gameTextList[3] + to_string(theScore); // updates temp score variable 
					// Determines which of the asteroids were destroyed and which need to be re-rendered
					//asteroidShot is used to do this in the renderer
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

		// Collison between player and obstacles
		for (vector<cAsteroid*>::iterator asteroidIterator = theAsteroids.begin(); asteroidIterator != theAsteroids.end(); ++asteroidIterator)
		{
			if ((*asteroidIterator)->collidedWith(&(*asteroidIterator)->getBoundingRect(), &(theRocket.getBoundingRect())))
			{
				theGameState = END; // Activates lose state if a collison betweeen the two has occured 
				//Resets some relevant variables 
				theRocket.setSpritePos({ 500, 550 });
				theScore -= theScore;
				theAreaClicked = { 0, 0 };
				collisionNum = 0; 

			}
		}
	
	}
	break;
	case END:
	{
		if (theButtonMgr->getBtn("exit_btn")->getClicked())
		{
		
		}
		
	}
	break;


	default:
		break;


	}

	

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
					
				}
				break;
			
				case SDLK_UP:
				{
					
				}
				break;
				//Player movement to the RHS of the screen
				case SDLK_d:
				{
					if (theRocket.getSpritePos().x < 789 && theGameState == PLAYING)
					{
						theRocket.setSpritePos({theRocket.getSpritePos().x +12 , theRocket.getSpritePos().y});
					}
				}
				break;
				//Player movemtn to the LHS of the screen
				case SDLK_a:
				{
					if (theRocket.getSpritePos().x > 152 && theGameState == PLAYING)
					{
						theRocket.setSpritePos({ theRocket.getSpritePos().x - 12, theRocket.getSpritePos().y });

					}
				}
				break;
				//Obtains user input for shooting 
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



