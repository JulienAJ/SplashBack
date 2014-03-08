class Splash
{
	private:
		int** board;
		int columns;
		int lines;
		int shots;
		int level;

		void generate();
		void action(int, int); // L, C
		void explode(int, int); // L, C
		void handle_action(int, int); //L, C

	public:
		Splash();
		Splash(int, int, int); // L, C, S
		~Splash();

		void play();
		void display();
		bool empty();
};
