import java.util.Random;
// This is fair because every BabyBird can take worm.
// To validate this, I make the ParentBird only put worm `rounds*birds/W` times
// and every BabyBird eats `round` times, if the program can end without deadlocks,
// that means every BabyBird takes enough worms, and ParentBird put enough worms.
// On the other hand, every BabyBird eats worm at a random sequence that differs 
// everytime. So it's fair.
class Dish {
    private int W = 20;
    private int value = W;
    private boolean empty = false;
    public Dish(int W){
        this.W = W;
        this.value = W;
    }
    public synchronized int take() {
        while (empty) {
            try {
                wait ();
            } catch (InterruptedException e) { }
        }
        value--;
        if (value == 0){
            empty = true;
            notifyAll ();
        }
        return value;
    }
    public synchronized void put() {
        while (!empty) {
            try {
                wait ();
            } catch (InterruptedException e) { }
        }
        this.value = W;
        empty = false;
        notifyAll ();
    }
}
class BabyBird extends Thread {
    int rounds;
    Dish dish;
    int id;
    Random r = new Random();
    public BabyBird(int rounds, Dish dish, int id) {
        this.rounds = rounds;
        this.dish = dish;
        this.id = id;
    }
    public void run() {
        for (int i = 0; i<rounds; i++) {
            try {
           	    sleep(r.nextInt(1000));
            } catch (InterruptedException e) {};
            int v = dish.take();
            System.out.println("BabyBird #"+this.id+" eats! Dish = "+v);
        }
    }
}

class ParentBird extends Thread {
    int rounds;
    Dish dish;
    Random r = new Random();
    public ParentBird(int rounds, Dish dish) {
        this.rounds = rounds;
        this.dish = dish;
    }
    public void run() {
        for (int i = 0; i<rounds; i++) {
            try {
           	    sleep(r.nextInt(1000));
            } catch (InterruptedException e) {};
            dish.put();
            System.out.println("ParentBird puts! ");
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
        new BabyBird(rounds, dish, i).start();
    new ParentBird(rounds*birds/W, dish).start();
  }
}
