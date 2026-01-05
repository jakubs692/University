package institutions;

import java.util.Arrays;
import java.util.Random;

/**
 * Represents a single lottery draw event.
 * Generates 6 unique random numbers.
 */
public class Draw {
    private final int number;
    private final int[] drawnNumbers;

    public Draw(int number) {
        this.number = number;
        this.drawnNumbers = drawUniqueNumbers();
        Arrays.sort(this.drawnNumbers);
    }

    /**
     * Constructor for testing purposes (allows setting specific numbers).
     */
    public Draw(int number, int[] numbers) {
        if (numbers.length != 6) {
            throw new IllegalArgumentException("Draw requires exactly 6 numbers.");
        }
        this.number = number;
        this.drawnNumbers = Arrays.copyOf(numbers, 6);
        Arrays.sort(this.drawnNumbers);
        if (!areUniqueAndInRange()) {
            throw new IllegalArgumentException("Numbers must be unique and in range 1-49.");
        }
    }

    private int[] drawUniqueNumbers() {
        Random rand = new Random();
        int[] numbers = new int[6];
        boolean[] used = new boolean[50]; // Indices 1-49
        for (int i = 0; i < 6; i++) {
            int randomNum;
            do {
                randomNum = rand.nextInt(49) + 1;
            } while (used[randomNum]);
            numbers[i] = randomNum;
            used[randomNum] = true;
        }
        return numbers;
    }

    private boolean areUniqueAndInRange() {
        boolean[] occurrence = new boolean[50];
        for (int num : drawnNumbers) {
            if (num < 1 || num > 49) return false;
            if (occurrence[num]) return false;
            occurrence[num] = true;
        }
        return true;
    }

    public int getNumber() { return number; }
    public int[] getDrawnNumbers() { return Arrays.copyOf(drawnNumbers, 6); }
}