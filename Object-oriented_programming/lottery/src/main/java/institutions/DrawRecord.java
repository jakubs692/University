package institutions;

/**
 * Stores the historical results of a draw, including winning numbers,
 * prize pools, individual win amounts, and the number of winners.
 */
public class DrawRecord {
    private final Draw draw;
    private long[] prizePools;
    private long[] winAmounts;
    private int[] winnerCounts;

    public DrawRecord(Draw draw, long[] prizePools, long[] winAmounts, int[] winnerCounts) {
        this.draw = draw;
        this.prizePools = prizePools;
        this.winAmounts = winAmounts;
        this.winnerCounts = winnerCounts;
    }

    public int[] getWinningNumbers() {
        return draw.getDrawnNumbers();
    }

    public String printWinningNumbers() {
        StringBuilder sb = new StringBuilder();
        for (int num : getWinningNumbers()) {
            sb.append(num).append(" ");
        }
        return sb.toString();
    }

    public void setPrizePools(long[] prizePools) { this.prizePools = prizePools; }
    public void setWinAmounts(long[] winAmounts) { this.winAmounts = winAmounts; }
    public void setWinnerCounts(int[] winnerCounts) { this.winnerCounts = winnerCounts; }

    public long[] getWinAmounts() { return winAmounts; }
    public int[] getWinnerCounts() { return winnerCounts; }

    /**
     * Prints the full report of the draw.
     */
    public String print() {
        StringBuilder sb = new StringBuilder();

        sb.append("Draw no. ").append(draw.getNumber()).append("\n");
        sb.append("Winning numbers: ").append(printWinningNumbers()).append("\n");

        sb.append("Prize pools per category (I-IV): \n");
        sb.append("I: ").append(prizePools[0] / 100).append(" PLN ").append(prizePools[0] % 100).append(" gr\n");
        sb.append("II: ").append(prizePools[1] / 100).append(" PLN ").append(prizePools[1] % 100).append(" gr\n");
        sb.append("III: ").append(prizePools[2] / 100).append(" PLN ").append(prizePools[2] % 100).append(" gr\n");
        sb.append("IV: ").append(prizePools[3] / 100).append(" PLN ").append(prizePools[3] % 100).append(" gr\n");

        sb.append("Number of winners per category:\n");
        sb.append("I: ").append(winnerCounts[0]).append("\n");
        sb.append("II: ").append(winnerCounts[1]).append("\n");
        sb.append("III: ").append(winnerCounts[2]).append("\n");
        sb.append("IV: ").append(winnerCounts[3]).append("\n");

        sb.append("Prize amount per winning bet:\n");
        sb.append("I: ").append(winAmounts[0] / 100).append(" PLN ").append(winAmounts[0] % 100).append(" gr\n");
        sb.append("II: ").append(winAmounts[1] / 100).append(" PLN ").append(winAmounts[1] % 100).append(" gr\n");
        sb.append("III: ").append(winAmounts[2] / 100).append(" PLN ").append(winAmounts[2] % 100).append(" gr\n");
        sb.append("IV: ").append(winAmounts[3] / 100).append(" PLN ").append(winAmounts[3] % 100).append(" gr\n");

        return sb.toString();
    }
}