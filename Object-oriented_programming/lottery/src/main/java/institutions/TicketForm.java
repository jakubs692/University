package institutions;

/**
 * Represents a physical blank ticket form (bet slip) filled out by a player.
 * Players mark numbers and the number of draws they wish to participate in.
 */
public class TicketForm {

    private final boolean[][] betFields; // 8 fields, numbers 1-49
    private final boolean[] canceledFields;
    private final boolean[] markedDraws; // How many draws (1-10)

    public TicketForm() {
        betFields = new boolean[8][50]; // Index 0 unused, 1-49 used
        canceledFields = new boolean[8];
        markedDraws = new boolean[10];
    }

    /**
     * Marks a specific number in a specific bet field.
     * @param fieldNumber The index of the bet field (1-8).
     * @param number The number to mark (1-49).
     */
    public void markNumber(int fieldNumber, int number) {
        assert fieldNumber > 0 && fieldNumber <= 8 : "Field numbers must be between 1-8. Provided: " + fieldNumber;
        assert number > 0 && number < 50 : "Numbers must be between 1-49. Provided: " + number;
        betFields[fieldNumber - 1][number] = true;
    }

    public void unmarkNumber(int fieldNumber, int number) {
        assert fieldNumber > 0 && fieldNumber <= 8 : "Field numbers must be between 1-8.";
        assert number > 0 && number < 50 : "Numbers must be between 1-49.";
        betFields[fieldNumber - 1][number] = false;
    }

    /**
     * Cancels a specific bet field, making it invalid.
     * @param fieldNumber The field to cancel (1-8).
     */
    public void cancelField(int fieldNumber) {
        assert fieldNumber > 0 && fieldNumber <= 8 : "Field numbers must be between 1-8.";
        canceledFields[fieldNumber - 1] = true;
    }

    public void markNumberOfDraws(int count) {
        assert count > 0 && count <= 10 : "Number of draws must be in range [1,10]. Provided: " + count;
        markedDraws[count - 1] = true;
    }

    /**
     * Retrieves the highest number of draws marked on the form.
     * @return Number of draws (defaults to 1 if none marked).
     */
    public int getNumberOfDraws() {
        for (int i = 9; i >= 0; i--) {
            if (markedDraws[i]) {
                return i + 1;
            }
        }
        return 1;
    }

    /**
     * Checks if a specific field contains exactly 6 marked numbers.
     * @param fieldNumber The field to check.
     * @return True if valid, false otherwise.
     */
    public boolean isValidBet(int fieldNumber) {
        assert fieldNumber > 0 && fieldNumber <= 8 : "Field numbers must be between 1-8.";
        int count = 0;
        for (int i = 1; i <= 49; i++) {
            if (betFields[fieldNumber - 1][i]) count++;
        }
        return count == 6;
    }

    /**
     * Counts how many valid bets (sets of 6 numbers) are on the form.
     * Ignores canceled fields.
     */
    public int countValidBets() {
        int validCount = 0;
        for (int i = 1; i <= 8; i++) {
            if (isValidBet(i) && !canceledFields[i - 1]) {
                validCount++;
            }
        }
        return validCount;
    }

    public int[] getBet(int fieldNumber) {
        assert fieldNumber > 0 && fieldNumber <= 8 : "Field numbers must be between 1-8.";
        int[] bet = new int[6];
        int k = 0;
        for (int i = 0; i < 50; i++) {
            if (this.betFields[fieldNumber - 1][i]) {
                if (k < 6) bet[k++] = i;
            }
        }
        return bet;
    }
}