import java.util.Random;
import java.util.concurrent.locks.Condition;
import java.util.concurrent.locks.Lock;
import java.util.concurrent.locks.ReentrantLock;

class Bathroom {  
  int nm = 0, nw = 0, dm = 0, dw = 0;
  final Lock lock = new ReentrantLock();
  final Condition man = lock.newCondition();
  final Condition woman = lock.newCondition(); 
  public void manEnter() {
    lock.lock();
    if (nw > 0 || dw > 0){
      dm ++;
      try { 
        //System.out.println("man wait");
        man.await();
      } catch (InterruptedException ex) {lock.unlock();return;}
    }
    nm ++;
    if (dm > 0){
      dm --;
      man.signalAll();
    }
    lock.unlock();
  }
  public void manExit() {
    lock.lock();
    nm --;
    if (nm == 0 && dw > 0){
      dw --;
      woman.signal();
    }
    lock.unlock();
  }
  public void womanEnter() {
    lock.lock();
    if (nm > 0 || dm > 0){
      dw ++;
      try { 
        woman.await();
      } catch (InterruptedException ex) {lock.unlock();return;}
    }
    nw ++;
    if (dw > 0){
      dw --;
      woman.signal();
    }
      lock.unlock();
  }
  public void womanExit() {
    lock.lock();
    nw --;
    if (nw == 0 && dm > 0){
      dm --;
      man.signal();
    }
      lock.unlock();
  }
}
  
class Woman extends Thread {
  int rounds;
  Bathroom br;
  int id;
  long starttime;
  Random r = new Random();
  public Woman(int id, int rounds, Bathroom br, long starttime) {
    this.rounds = rounds;
    this.br = br;
    this.id = id;
    this.starttime = starttime;
  }
  public void run() {
    for (int i = 0; i<rounds; i++) {
      try {
        System.out.println("[" + (System.currentTimeMillis()-starttime) +"]woman #" + this.id + " waits");
        br.womanEnter();
        System.out.println("[" + (System.currentTimeMillis()-starttime) +"]woman #" + this.id + " enter");
        Thread.sleep(r.nextInt(1000));
        br.womanExit();
        System.out.println("[" + (System.currentTimeMillis()-starttime) +"]woman #" + this.id + " exit ");
        Thread.sleep(r.nextInt(1000));
      } catch (InterruptedException e) {e.printStackTrace();};
    }
  }
}

class Man extends Thread {
  int rounds;
  Bathroom br;
  int id;
  long starttime;
  Random r = new Random();
  public Man(int id, int rounds, Bathroom br, long starttime) {
    this.rounds = rounds;
    this.br = br;
    this.id = id;
    this.starttime = starttime;
  }
  public void run() {
    for (int i = 0; i<rounds; i++) {
      try {
        System.out.println("[" + (System.currentTimeMillis()-starttime) +"]man #" + this.id + " waits");
        br.manEnter();
        System.out.println("[" + (System.currentTimeMillis()-starttime) +"]man #" + this.id + " enter");
        Thread.sleep(r.nextInt(1000));
        br.manExit();
        System.out.println("[" + (System.currentTimeMillis()-starttime) +"]man #" + this.id + " exit ");
        Thread.sleep(r.nextInt(1000));
      } catch (InterruptedException e) { e.printStackTrace();};
    }
  }
}


class Main {
  static Bathroom br = new Bathroom();
  public static void main(String[] arg) {
    int rounds = Integer.parseInt(arg[0],10);
    int count = Integer.parseInt(arg[1],10);
    int i;
    long starttime = System.currentTimeMillis();
    for(i=0; i<count; i++){
      new Man(i, rounds, br, starttime).start();
    }
    for(i=0; i<count; i++){
      new Woman(i, rounds, br, starttime).start();
    }
  }
}
