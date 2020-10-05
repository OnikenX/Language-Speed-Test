import java.util.Vector;

class javatests {
    private static Vector<Integer> primenumbers(){
        return primenumbers(1000);
    }
    private static Vector<Integer> primenumbers(Integer max) {
        Vector<Integer> primes = new Vector<Integer>();
        primes.add(1);
        primes.add(2);
        Integer div, actual = 3;
        Boolean notprime;
        while (actual < max) {
            notprime = false;
            for (div = 2; div < actual; ++div)
              if ((actual % div) == 0) {
                notprime = true;
                break;
              }
            if (!notprime)
              primes.add(actual);
            ++actual;
          }
        return primes;
    }

    public static void main(String[] args) {
        Vector<Integer> primes = primenumbers();
        primes.forEach(prime -> {
            System.out.println(prime);
        });

    }
}
