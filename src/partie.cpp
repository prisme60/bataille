#include <list>
#include <vector>
#include <stack>
#include <map>
//#include <iterator>
#include <algorithm>

#include <iostream>

#include <cstdlib>
#include <ctime>

#include <cassert>

#include <chrono>

#include "card.h"

using namespace std;

typedef int tIdPlayer;

class game
{
  public:
   game(int number_of_Player = 2);
   void distribution();
   void addPlayer();

   void play();

   void turn();

   static void printCardHEX(tCard card);

  private:
    bool refreshActivePlayers();

  private:
   int numberOfPlayer;
   vector <list<tCard> > cardsStackOfPlayers;
   list <tIdPlayer> listOfActivePlayersDuringTurn;
};

class cardGenerator
{
  public:
    cardGenerator();
    tCard operator()();

  private:
    tCard card;
};

class CardPlayerPair {
  public:
  CardPlayerPair(tCard _card, tIdPlayer _idPlayer): card(_card), idPlayer(_idPlayer) {}

  //this operator is used for sort method
  virtual bool operator<(const CardPlayerPair & c)
  {
    if(getValue(c.card)==1)
      return getValue(card) != 1;
    else if(getValue(card)==1)
      return false; //c.card can't be an ace, because it is already treat in the first condition
    else
      return getValue(card) < getValue(c.card);
  }

  //this operator is sued for remove method (need eduqlity when searching the element)
  virtual bool operator==(const CardPlayerPair & c)
  {
      return ((card == c.card) && (idPlayer== c.idPlayer));
  }

  inline tCard getCard()  {return card;}
  inline tIdPlayer getIdPlayer()  {return idPlayer;}

  private:
    tCard card;
    tIdPlayer idPlayer;
};

cardGenerator::cardGenerator()
{
  card = card_empty;
}

tCard cardGenerator::operator()()
{
  card = getNextCard(card);
  return card;
}

int main(int argc, char * argv[])
{
  int nbPlayer = 2;
  if(argc > 1)
  {  
    nbPlayer = atoi(argv[1]);
  }

  game partie(nbPlayer);

  partie.play();
}

game::game(int number_of_Player) : numberOfPlayer(number_of_Player)
{
  for(int i=0; i<numberOfPlayer; i++)
    addPlayer();
  
  distribution();
}

void game::addPlayer()
{
  cardsStackOfPlayers.push_back(list<tCard>());
}

void game::printCardHEX(tCard card)
{
  cout << hex << card << "\t";
}

void game::distribution()
{
  vector<tCard> newCardStack(NUMBER_OF_CARDS);
  
  generate(newCardStack.begin(),newCardStack.end(),cardGenerator());
  for_each(newCardStack.begin(),newCardStack.end(),printCardHEX);

  auto start = chrono::steady_clock::now();
  srand(chrono::duration_cast<chrono::milliseconds>(start.time_since_epoch()).count());

  random_shuffle(newCardStack.begin(), newCardStack.end());
  
  int currentPlayer = 0;
  for(vector<tCard>::iterator p = newCardStack.begin(); p != newCardStack.end(); p++)
  {
    cardsStackOfPlayers[currentPlayer++%numberOfPlayer].push_back(*p);
  }
}

void game::play()
{
    int nbTurn = 0;
    while(refreshActivePlayers())
    {
      turn();
      nbTurn++;
    }
    cout << "winner is player " << listOfActivePlayersDuringTurn.front() << " and there was " << dec << nbTurn << " turns." << endl;
}

void game::turn()
{
    stack<tCard> playedCards;
    list<CardPlayerPair> listOfCardPlayer;
    multimap<tCard,CardPlayerPair> listOfBataille;

    //prendre une carte de chaque joueur actif et les comparer (les mettre dans la structure tCardPlayerPair)
    for(list<tIdPlayer>::iterator pPlayer=listOfActivePlayersDuringTurn.begin();
      pPlayer != listOfActivePlayersDuringTurn.end();
      pPlayer++)
    {
	if(!cardsStackOfPlayers[*pPlayer].empty())
	{
	  cout << "Player " << *pPlayer << hex << " joue carte " << cardsStackOfPlayers[*pPlayer].front() << endl;
	  listOfCardPlayer.push_back(CardPlayerPair(cardsStackOfPlayers[*pPlayer].front(), *pPlayer));
	  cardsStackOfPlayers[*pPlayer].pop_front();
	}
	else
	{
	  cout << "Player " << *pPlayer << " n'a plus de carte." << endl;
	}
    }
	
    bool bStillABataille=false;
    do
    {
	//tri par ordre croissant de valeur de carte (surcharge de l'operateur de comparaison <)
	listOfCardPlayer.sort();

	//Mettre en bataille les dernières valeurs égales, mais n'étant pas forcément maximales! (bataille il y aura , si il y en a plus qu'une!)
	//for(list<CardPlayerPair>::iterator pPair = listOfCardPlayer.rbegin(); pPair != listOfCardPlayer.rend(); pPair++)
	listOfBataille.clear();//all data from the previous batailles are cleared
        for(list<CardPlayerPair>::iterator pPair = listOfCardPlayer.begin(); pPair != listOfCardPlayer.end(); pPair++)
	{
	    tCard card_currentValue = getValue(pPair->getCard());
	    cout << hex << "Carte retenue " << pPair->getCard() << " idPlayer=" << pPair->getIdPlayer() << endl;
	    listOfBataille.insert(multimap<tCard,CardPlayerPair>::value_type(card_currentValue,*pPair));
	}

	bStillABataille=false;
	//itérer sur les batailles
	for(tCard cardValue = value_as; cardValue <= value_K ; cardValue = getNextCard(cardValue))
	{
	  if(listOfBataille.count(cardValue)>=2)
	  {//A bataille can be done only if there is 2 cards with the same value!
	    for(multimap<tCard,CardPlayerPair>::iterator pPair=listOfBataille.lower_bound(cardValue);
	      pPair != listOfBataille.upper_bound(cardValue);
	      pPair++)
	    {//les batailleurs doivent "payer" de leur carte courrante, et se démettre de la carte du dessus du tas les playedCards
	      tIdPlayer currentIdPlayer = pPair->second.getIdPlayer();
	      playedCards.push(pPair->second.getCard());//la carte posée est placée dans les cartes jouées (et sera récupérée par le gagnant du tour)
	      if(!cardsStackOfPlayers[currentIdPlayer].empty())//avant de prendre une carte du tas du joueur, on vérifie que celui-ci n'est pas vide
	      {
		playedCards.push(cardsStackOfPlayers[currentIdPlayer].front());//la carte est mise dans les cartes jouées (sans même avoir été regarder!)
		cardsStackOfPlayers[currentIdPlayer].pop_front();//la carte est retirée du tas du joueur

		//les batailleurs doivent piocher une nouvelle carte de le dessus de leur tas et remplacer celle posée sur la table
		listOfCardPlayer.remove(pPair->second);//Dans les 2 cas on retire, le joueur de la liste avec sa carte courrante
		if(!cardsStackOfPlayers[currentIdPlayer].empty())//avant de prendre une carte du tas du joueur, on vérifie que celui-ci n'est pas vide
		{
		  listOfCardPlayer.push_back(CardPlayerPair(cardsStackOfPlayers[currentIdPlayer].front(),currentIdPlayer));//la carte du dessus du tas est prise, et est mise en jeu
		  cardsStackOfPlayers[currentIdPlayer].pop_front();//la carte est retirée du tas du joueur
		  //les batailleurs doivent piocher une nouvelle carte de le dessus de leur tas et remplacer celle posée sur la table
		  bStillABataille=true;
		}
		else
		{
		  //le joueur a été retiré de listOfCardPlayer, afin de ne plus en entendre parler!!!! Pas question de l'y remettre, quand on a plus de carte, on n'a plus de carte!
		  cout << "Player " << pPair->first << " n'a plus de carte pour payer la bataille." << endl;
		}
	      }
	      else
	      {
		cout << "Player " << pPair->first << " n'a plus de carte pour payer la bataille." << endl;
		//Il faut retirer le joueur des listOfCardPlayer, afin de ne plus en entendre parler!!!! TU AS PERDU
		listOfCardPlayer.remove(pPair->second);
	      }
	    }
	  }
	  //la comparaison peut recommencer
	}
	
	//effacement des cartes actuellement joués lors du dernier affrontement, puis qu'elles ont été recopié dans la liste des cartes jouées
//	listOfCardPlayer.clear();
    } while( bStillABataille);

    //reporter les cartes jouer durant le tour dans playedCards
    for(list<CardPlayerPair>::iterator pPair = listOfCardPlayer.begin(); pPair != listOfCardPlayer.end(); pPair++)
    {
	playedCards.push(pPair->getCard());
    }

    //le gagnant du tour gagne toutes les cartes jouées pendant le tour!
    if(listOfBataille.size()>0)
    {
	for(multimap<tCard,CardPlayerPair>::iterator pPair=listOfBataille.begin();
		      pPair != listOfBataille.end();
		      pPair++)
	{
	  cout << hex << "[" << pPair->first << "," << pPair->second.getCard() << "]" << endl;
	}

	tIdPlayer turnWinnerIdPLayer;
	if(listOfBataille.count(value_as)>0)
	  turnWinnerIdPLayer = listOfBataille.begin()->second.getIdPlayer();//take the first element, because one player has played an ACE
	else
	  turnWinnerIdPLayer = listOfBataille.rbegin()->second.getIdPlayer();//take the last element (K,Q,J,10,...), because no ACE has been played 
	cout << "le gagnant du tour est le joueur "<< turnWinnerIdPLayer << " et récupère les cartes suivantes \t";
	while(!playedCards.empty())
	{
	  cardsStackOfPlayers[turnWinnerIdPLayer].push_back(playedCards.top());//ajoute la carte au joueur
	  cout << playedCards.top() << "\t";
	  playedCards.pop();//la retire des cartes jouées pendant le tour
	}
	cout << "\n" << endl;
    }
    else
    {
      cout << "il semblerait que plus aucun joueur n'ait de cartes! Partie nulle? Y aurait pas un bug?" << endl;
      assert(listOfBataille.size() > 0);
    }
}

bool game::refreshActivePlayers()
{
  tIdPlayer idPLayer = 0;
  listOfActivePlayersDuringTurn.clear();
  for(vector<list<tCard> >::iterator pCardStackOfPlayer=cardsStackOfPlayers.begin();
      pCardStackOfPlayer != cardsStackOfPlayers.end();
      pCardStackOfPlayer++, idPLayer++)
  {
    if(!pCardStackOfPlayer->empty())
	listOfActivePlayersDuringTurn.push_back(idPLayer);
  }
  return (listOfActivePlayersDuringTurn.size() > 1);
}
