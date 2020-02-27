import java.util.Random;

// This is fair because every HoneyBees can put honey in the pot.
// To validate this, I make the Bear only eat `rounds*bees/W` times
// and every HoneyBees put honey `round` times, if the program can end without deadlocks,
// that means every HoneyBees puts enough honey, and Bear eat enough honey.
// On the other hand, every HoneyBees put honey at a random sequence that differs 
// everytime. So it's fair.
class Dish {
    private int W = 20;
    private int value = 0;
    private boolean full = false;
    public Dish(int W){
        this.W = W;
    }
    public synchronized void take() {
        while (!full) {
            try {
                wait ();
            } catch (InterruptedException e) { }
        }
        value = 0;
        full = false;
        notifyAll ();
    }
    public synchronized int put() {
        while (full) {
            try {
                wait ();
            } catch (InterruptedException e) { }
        }
        value ++;
        if(value == W){
            full = true;
            notifyAll ();
        }
        return value;
    }
}
class HoneyBee extends Thread {
    int rounds;
    Dish dish;
    int id;
    Random r = new Random();
    public HoneyBee(int rounds, Dish dish, int id) {
        this.rounds = rounds;
        this.dish = dish;
        this.id = id;
    }
    public void run() {
        for (int i = 0; i<rounds; i++) {
            try {
           	    sleep(r.nextInt(1000));
            } catch (InterruptedException e) {};
            int v = dish.put();
            System.out.println("HoneyBee #"+this.id+" puts! Pot = "+v);
        }
    }
}

class Bear extends Thread {
    int rounds;
    Dish dish;
    Random r = new Random();
    public Bear(int rounds, Dish dish) {
        this.rounds = rounds;
        this.dish = dish;
    }
    public void run() {
        for (int i = 0; i<rounds; i++) {
            try {
           	    sleep(r.nextInt(1000));
            } catch (InterruptedException e) {};
            dish.take();
            System.out.println("Bear eats! "+i);
        }
    }
}

class Main {  // driver program -- two readers and one writer
  public static void main(String[] arg) {
    int rounds = Integer.parseInt(arg[0],10);
    int birds = Integer.parseInt(arg[1],10);
    int W = Integer.parseInt(arg[2],10);
    Dish dish = new Dish(W);
    int i;
    for (i=0; i<birds;i++)
        new HoneyBee(rounds, dish, i).start();
    new Bear(rounds*birds/W, dish).start();
  }
}
