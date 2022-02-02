/*===============================================================
// Nom du fichier : b_automatix_Zakaria_HAJJI
// Auteur : Zakaria HAJJI
// Date de creation : 21/03/2020
// Version : Version final
// ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
// Manuel d'utilisation :
//
// Changer de mode :
// - Par défaut manuel
// - Appuyer successivement deux fois sur la touche « haut ».
// - Un signal sonore ainsi qu’un affichage confirmera le changement de mode.
//
// Mode Manuel :
// - Appuier sur le capteur de contact pour faire monter/descendre la barriere.
// **Attention : il s’agit d’un appuie simple et non d’un appui prolonge.**
// - Lorsque aucun vehicule n'est détecte celle-ci se referme.
// - Lorsque l'on ouvre la barriere un signal sonore est emis.
//
// Mode Automatique :
// - Lorsque la barriere détecte du bruit ainsiqu’un vehicule, la brique vous invite a saisir un code.
// - La barriere s’ouvre si celui-ci est bon.
// - Elle se referme automatiquement une fois qu’aucune présence n'est détecte.
//
// Code d’accès :
// - Appuyer dans l’ordre sur les boutons gauche, droite, gauche, bas de la brique.
// --------------------------------------------------------------
// A noter :
// - Cette barriere est uniquement une barriere d'entree.
// - Le son emis par le vehicule sera d'un minimum de 10 Db.
// - Les capteurs ultrasons captent à une distance de 10cm devant eux.
// - La capteur de contact se branche en 1.
// - La zone 1 de la barriere est défini par le capteur US branche en 2, la zone 2 en 3
// - Le capteur de son est branche en 4.
// - Le moteur est branche en A.
//===============================================================*/

/*----------Fonction qui permet lors de son appel l'enregistrement de l'etat des deux capteurs US------------------ */
int US()
{
	float distance1,distance2;
	int etat;
	distance1=getUSDistance(S2);
	distance2=getUSDistance(S3);
	if( distance1<10 && distance2<10)
	{
		etat=3;
	}
	else if(distance1>10 && distance2<10)
	{
		etat=2;
	}
	else if(distance1<10 && distance2>10)
	{
		etat=1;
	}
	else
	{
		etat=0;
	}
	return etat;
}
/*---------------------------------Fin de la fonction d'enregistrement des US-------------------------------------- */

/*----------------Tasks qui permettent d'afficher un logo et de produire un son au demarrage----------------------- */
task logo()
{
		drawBmpfile(0,127,"Warning");
		sleep(800);
}
task logoSound()
{
		playSoundFile("Start");
		sleep(1000);
}
/*-------------------------------------------------Fin des tasks--------------------------------------------------- */

/*----------------------------------------------- Corps du Programme----------------------------------------------- */
task main()
{
	int cptManuel=0,etatUS,cptAuto=0,etatBarriere=0,etatMode=0,cptMode=0,cptVoiture=0;
	startTask(logoSound);
	startTask(logo);
	stopTask(logoSound);
	stopTask(logo);
	sleep(1000);
	eraseDisplay();
	while(1)
	{
		setSoundVolume(50);
/*----------------------------------------------------MANUEL-------------------------------------------------------*/
		while(etatMode==0)
		{
			displayCenteredTextLine(5, "Mode : Manuel");
			displayTextLine(15, "Nombre de voiture passÃ©e : %d",cptVoiture);
			SensorType[S4] = sensorSoundDB;
			SensorType[S1] = sensorEV3_Touch;
			etatUS=US();
			if(cptManuel==0 && SensorValue[S1]==1 && etatUS!=3 )/* Ouvrir la barriere manuellement */
			{
				playSound(soundDownwardTones);  //prevenir l'ouverture du barriere
				sleep(400);
				setMotorTarget(motorA,90,20);
				sleep(400);
				cptManuel=1;
			}
			if(cptManuel==1 && SensorValue[S1]==1 && etatUS==0) /* Fermer la barriere manuellement */
			{
				setMotorTarget(motorA,0,10);
				sleep(400);
				cptManuel=0;
			}
			if(getButtonPress(buttonUp)==true) /* Changement de mode avec signal sonore */
			{
				sleep(400);
				cptMode=1;
				if((cptMode==1) && (getButtonPress(buttonUp)==true))
				{
					sleep(400);
					etatMode=1;
					cptMode=0;
					playSound(soundBeepBeep);
					eraseDisplay();
				}
				else if(getButtonPress(buttonRight)==true || getButtonPress(buttonLeft)==true || getButtonPress(buttonDown)==true || getButtonPress(buttonEnter)==true || getButtonPress(buttonBack)==true)
				{
					cptMode=0;
					displayCenteredTextLine(12, "Code faux");
					sleep(1000);
				}
			}
		}
		if(etatUS==3)
		{
			sleep(1000);
			cptVoiture=cptVoiture+1;
		}
/*----------------------------------------------------FIN MANUEL-------------------------------------------------------*/

/*----------------------------------------------------AUTOMATIQUE-------------------------------------------------------*/
		while(etatMode==1)
		{
			displayCenteredTextLine(5, "Mode : Automatique");
			SensorType[S4] = sensorSoundDB;
			SensorType[S1] = sensorEV3_Touch;
			displayTextLine(15, "Nombre de voiture passÃ©e : %d",cptVoiture);
			etatUS=US();
			while (etatUS!=0 && SensorValue[S4]>= 10) /* Vérification de la présence et du bruit avant d'autoriser à rentrer le code */
			{
				displayCenteredTextLine(6, "Entrez le code");
				if(cptAuto==0 && getButtonPress(buttonLeft)==true)
				{
					cptAuto=1;
				}
				if((cptAuto==1) && (getButtonPress(buttonRight)==true))
				{
					cptAuto=2;
				}
				if(cptAuto==2 && getButtonPress(buttonLeft)==true)
				{
					cptAuto=3;
				}
				if((cptAuto==3) && (getButtonPress(buttonDown)==true))
				{
					cptAuto=4;
				}
				if (etatUS==1 && cptAuto==4 && SensorValue[S4]>= 10 )  /*Verification du code et de l'état des capteurs et le son pour l'ouverture*/
				{
					setMotorTarget(motorA,90,20);
					displayCenteredTextLine(7, "Code bon : ouverture");
					sleep(2000);
					etatBarriere=1;
					cptAuto=0;
					displayCenteredTextLine(7, "");
					cptVoiture=cptVoiture+1;
				}
			}
			etatUS=US();
			if (etatUS==0 && etatBarriere==1) /* Verification du code et de l'etat des capteurs pour la fermeture */
			{
				eraseDisplay();
				setMotorTarget(motorA,0,10);
				sleep(400);
				etatBarriere=0;
			}
			if(getButtonPress(buttonUp)==true) /* Changement de mode avec signal sonore */
			{
				sleep(400);
				cptMode=1;
				if((cptMode==1) && (getButtonPress(buttonUp)==true))
				{
					sleep(400);
					etatMode=0;
					cptMode=0;
					playSound(soundBeepBeep);
					eraseDisplay();
				}
				else if(getButtonPress(buttonRight)==true || getButtonPress(buttonLeft)==true || getButtonPress(buttonDown)==true || getButtonPress(buttonEnter)==true || getButtonPress(buttonBack)==true)
				{
					cptMode=0;
					displayCenteredTextLine(12, "Code faux");
					sleep(1000);
				}
			}
		}
/*----------------------------------------------------FIN AUTOMATIQUE-------------------------------------------------------*/
	}
}
/*----------------------------------------------- Fin du corps du Programme------------------------------------------------ */