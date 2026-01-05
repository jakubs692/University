package institutions;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import static java.lang.Math.max;
import static java.lang.Math.min;

/**
 * The Central Lottery Office. Manages draws, outlets, and finances.
 */
public class Headquarters {
    private final StateTreasury stateTreasury;
    private long centralProfits;
    private final ArrayList<LotteryOutlet> outlets;
    private int conductedDrawsCount;
    private final HashMap<Integer, DrawRecord> drawHistory;
    private int ticketsIssued;
    private final HashMap<Integer, List<Ticket>> ticketsInDraw;
    private final HashMap<Integer, Long> fundsPerDraw;
    private long jackpotAccumulation;

    public Headquarters(StateTreasury treasury) {
        assert treasury != null;
        this.stateTreasury = treasury;
        this.outlets = new ArrayList<>();
        this.conductedDrawsCount = 0;
        this.drawHistory = new HashMap<>();
        this.ticketsIssued = 0;
        this.ticketsInDraw = new HashMap<>();
        this.fundsPerDraw = new HashMap<>();
        this.jackpotAccumulation = 0;
        
        // Initialize for future draws
        for (int i = 1; i < 20; i++) {
            ticketsInDraw.put(i, new ArrayList<>());
            fundsPerDraw.put(i, 0L);
        }
    }

    public void addOutlet() {
        this.outlets.add(new LotteryOutlet(outlets.size(), this));
    }

    public ArrayList<LotteryOutlet> getOutlets() { return outlets; }
    
    public int getNextTicketNumber() { return ++ticketsIssued; }
    
    public int getConductedDrawsCount() { return conductedDrawsCount; }

    public void addTicket(Ticket ticket) {
        if (ticket == null) return;
        for (int drawNum : ticket.getDrawNumbers()) {
            if (!ticketsInDraw.containsKey(drawNum)) {
                ticketsInDraw.put(drawNum, new ArrayList<>());
            }
            ticketsInDraw.get(drawNum).add(ticket);
        }
    }

    public void transferTicketFees(Ticket ticket) {
        long revenuePart = (ticket.getPrice() - ticket.getTax()) / ticket.getNumberOfDraws();
        for (int drawNum : ticket.getDrawNumbers()) {
            fundsPerDraw.put(drawNum, fundsPerDraw.getOrDefault(drawNum, 0L) + revenuePart);
        }
        payTaxToBudget(ticket.getTax());
    }

    public void payTaxToBudget(long amount) {
        stateTreasury.collectTaxes(amount);
    }

    /**
     * Conducts a new lottery draw.
     * Calculates winners, prizes, and updates history.
     */
    public void conductDraw() {
        int drawNumber = conductedDrawsCount + 1;
        Draw newDraw = new Draw(drawNumber);

        DrawRecord record = new DrawRecord(newDraw, new long[4], new long[4], new int[4]);
        drawHistory.put(drawNumber, record);

        // Count winners
        int[] winnerCounts = new int[4];
        if (ticketsInDraw.containsKey(drawNumber)) {
            for (Ticket ticket : ticketsInDraw.get(drawNumber)) {
                int[] hits = ticket.checkHits(this, drawNumber);
                for (int i = 0; i < 4; i++) winnerCounts[i] += hits[i];
            }
        }

        // Calculate money
        long funds = fundsPerDraw.getOrDefault(drawNumber, 0L);
        long prizeFund = (51 * funds) / 100;
        centralProfits += (funds - prizeFund);

        // Distribute prizes
        long[] prizePools = calculatePrizeDistribution(prizeFund, winnerCounts);
        long[] winAmounts = new long[4];
        for (int i = 0; i < 4; i++) {
            winAmounts[i] = prizePools[i] / max(winnerCounts[i], 1);
        }

        // Handle jackpot
        if (winnerCounts[0] == 0) {
            this.jackpotAccumulation = winAmounts[0];
        } else {
            this.jackpotAccumulation = 0;
        }

        record.setPrizePools(prizePools);
        record.setWinAmounts(winAmounts);
        record.setWinnerCounts(winnerCounts);

        // Prepare next draw slot
        int nextDraw = drawNumber + 10;
        if (!ticketsInDraw.containsKey(nextDraw)) {
            ticketsInDraw.put(nextDraw, new ArrayList<>());
            fundsPerDraw.put(nextDraw, 0L);
        }

        this.conductedDrawsCount++;
    }
    
    private long[] calculatePrizeDistribution(long totalPrizeFund, int[] winners) {
        long[] pools = new long[4];

        pools[0] = Math.max((44 * totalPrizeFund) / 100, 200000000); // Min jackpot
        pools[1] = (8 * totalPrizeFund) / 100;
        pools[3] = 2400L * winners[3]; // Guaranteed 24 PLN for 3 hits
        
        long remainder = totalPrizeFund - pools[1] - pools[3] - (44 * totalPrizeFund / 100);
        pools[2] = Math.max(remainder, 3600L * winners[2]);

        pools[0] += this.jackpotAccumulation;
        return pools;
    }

    public int[] getWinningNumbers(int drawNumber) {

        if (drawHistory.containsKey(drawNumber)) {
            return drawHistory.get(drawNumber).getWinningNumbers();
        }
        return new int[6]; 
    }

    public long[] getWinAmounts(int drawNumber) {
        return drawHistory.get(drawNumber).getWinAmounts();
    }
    
    public void updateFundsAfterPayout(int drawNum, long amountPaid) {
        long current = fundsPerDraw.get(drawNum);
        long updated = current - amountPaid;
        
        if (updated < 0) {
            long loan = min(centralProfits, -updated);
            centralProfits -= loan;
            updated += loan;
        }
        if (updated < 0) {
            long subsidy = stateTreasury.grantSubsidy(-updated);
            updated += subsidy;
        }
        fundsPerDraw.put(drawNum, updated);
    }
    
    public void removeTicket(Ticket ticket) {
        for (int drawNum : ticket.getDrawNumbers()) {
            if(ticketsInDraw.containsKey(drawNum)) {
                ticketsInDraw.get(drawNum).remove(ticket);
            }
        }
    }

    public void printHistory() {
        for (DrawRecord record : drawHistory.values()) {
            System.out.println(record.print());
        }
    }
    
    public ArrayList<LotteryOutlet> getPodlegajaceKolektury() { return getOutlets(); } // Bridge for legacy naming if needed
}