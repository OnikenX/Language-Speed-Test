def primenumbers(max: int = 100000):
    primes = [1,2]
    actual = 3
    while actual < max:
        for div in range(2, actual):
            if not (actual % div):
                break
        else:
            primes.append(actual)
        actual += 1
    return primes
                
if __name__ == "__main__":
    #primes = primenumbers()
    for prime in primenumbers():
        print(prime)

