package institutions;

import java.util.ArrayList;
import java.util.Random;

/**
 * Represents an official lottery ticket sold by an outlet.
 * Contains the bets, the unique ID, and tax/price information.
 */
public class Ticket {
    private final String id;
    private final ArrayList<int[]> bets;
    private final ArrayList<Integer> drawNumbers;
    private final long price;
    private final long tax;
    private final LotteryOutlet issuingOutlet;

    public Ticket(LotteryOutlet issuingOutlet, int ticketNumber, ArrayList<int[]> bets,
                  int numberOfDraws, int nextDrawNumber) {
        assert issuingOutlet != null : "Issuing outlet cannot be null.";
        assert bets != null : "Bets list cannot be null.";
        assert numberOfDraws > 0 && numberOfDraws <= 10 : "Tickets can be valid for 1-10 draws.";
        
        this.issuingOutlet = issuingOutlet;
        this.bets = new ArrayList<>(bets);
        this.drawNumbers = generateDrawNumbers(nextDrawNumber, numberOfDraws);
        this.id = generateId(issuingOutlet.getOutletNumber(), ticketNumber);
        this.price = calculatePrice(bets.size(), numberOfDraws);
        this.tax = calculateTax(bets.size(), numberOfDraws);
    }

    public LotteryOutlet getIssuingOutlet() {
        return issuingOutlet;
    }

    public int getNumberOfBets() {
        return this.bets.size();
    }

    public int getNumberOfDraws() {
        return this.drawNumbers.size();
    }

    /**
     * Checks how many bets on this ticket won in a specific draw.
     * @param headquarters The central office (source of winning numbers).
     * @param drawNumber The draw number to check against.
     * @return Array where index i represents count of wins of degree (i+1). 
     * [0]=6 hits, [1]=5 hits, [2]=4 hits, [3]=3 hits.
     */
    public int[] checkHits(Headquarters headquarters, int drawNumber) {
        assert headquarters != null : "Headquarters cannot be null.";
        int[] winsPerCategory = new int[4];
        int[] winningSix = headquarters.getWinningNumbers(drawNumber);

        for (int[] bet : this.bets) {
            int hits = 0;
            int betIdx = 0;
            int winIdx = 0;
            
            // Compare sorted arrays
            while (betIdx < 6 && winIdx < 6) {
                if (bet[betIdx] == winningSix[winIdx]) {
                    hits++;
                    winIdx++;
                    betIdx++;
                } else if (bet[betIdx] > winningSix[winIdx]) {
                    winIdx++;
                } else {
                    betIdx++;
                }
            }
            
            // Map hits to categories: 6->0, 5->1, 4->2, 3->3
            if (hits >= 3) {
                winsPerCategory[6 - hits]++;
            }
        }
        return winsPerCategory;
    }

    private ArrayList<Integer> generateDrawNumbers(int start, int count) {
        ArrayList<Integer> numbers = new ArrayList<>();
        for (int i = 0; i < count; i++) {
            numbers.add(start + i);
        }
        return numbers;
    }

    private String generateId(int outletNumber, int ticketNumber) {
        Random rand = new Random();
        StringBuilder randomTag = new StringBuilder();
        for (int i = 0; i < 9; i++) {
            randomTag.append(rand.nextInt(10));
        }
        long sum = sumDigits(String.valueOf(ticketNumber))
                + sumDigits(String.valueOf(outletNumber))
                + sumDigits(randomTag.toString());
        int checksum = (int) (sum % 100);
        return String.format("%d-%d-%s-%02d", ticketNumber, outletNumber, randomTag, checksum);
    }

    private long sumDigits(String s) {
        return s.chars().filter(Character::isDigit).map(c -> c - '0').sum();
    }

    private long calculatePrice(int betsCount, int drawsCount) {
        return betsCount * drawsCount * 300L; // 3.00 PLN per bet per draw
    }

    private long calculateTax(int betsCount, int drawsCount) {
        return betsCount * drawsCount * 60L; // Tax portion
    }

    public String getId() { return id; }
    public ArrayList<Integer> getDrawNumbers() { return new ArrayList<>(drawNumbers); }
    public long getPrice() { return price; }
    public long getTax() { return tax; }

    @Override
    public String toString() {
        return "TICKET ID: " + id + ", Price: " + price/100 + " PLN";
    }
}