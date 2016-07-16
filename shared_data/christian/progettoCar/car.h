class Controller {
public:
  enum { LEFT = 0, RIGHT = 1, ACC = 2, DEC = 3, NKEYS = 4};
  bool key[NKEYS];
  void Init();
  void EatKey(int keycode, int* keymap, bool pressed_or_released);
  void Joy(int keymap, bool pressed_or_released);
  Controller() {Init();} // costruttore
};


class Car {

  void RenderAllParts(bool usecolor, bool allParts) const;
  // disegna tutte le parti della macchina
  // invocato due volte: per la car e la sua ombra

public:
  // Metodi
  void Init(); // inizializza variabili
  void Render() const; // disegna a schermo
  void Render(bool allParts) const; // disegna a schermo
  void DoStep(); // computa un passo del motore fisico
  Car() {Init();} // costruttore

  void checkCollision(float px, float pz); // Verifica le collisioni tra macchina e goals
  void resetScore(); // Resetta il punteggio
  void updateDifficultyLevel(); // Effettua un update della difficoltà

  Controller controller;

  // STATO DELLA MACCHINA
  // (DoStep fa evolvere queste variabili nel tempo)
  float px, py, pz, facing; // posizione e orientamento
  float mozzoA, mozzoP, sterzo; // stato interno
  float vx, vy, vz; // velocita' attuale
  int goalsReached;
  int totalGoals;

  // STATS DELLA MACCHINA
  // (di solito rimangono costanti)
  float velSterzo, velRitornoSterzo, accMax, attrito,
        raggioRuotaA, raggioRuotaP, grip,
        attritoX, attritoY, attritoZ; // attriti

private:
  void DrawHeadlight(float x, float y, float z, int lightN, bool useHeadlight) const;
};
