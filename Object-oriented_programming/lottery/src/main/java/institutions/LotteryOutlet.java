package institutions;

import java.util.ArrayList;
import java.util.HashMap;

/**
 * Represents a local lottery outlet where players buy tickets and claim prizes.
 */
public class LotteryOutlet {
    private final int outletNumber;
    private final HashMap<Ticket, Boolean> soldTickets;
    private final Headquarters headquarters;

    public LotteryOutlet(int number, Headquarters headquarters) {
        assert headquarters != null : "Headquarters cannot be null.";
        this.outletNumber = number;
        this.soldTickets = new HashMap<>();
        this.headquarters = headquarters;
    }

    public int getOutletNumber() { return outletNumber; }
    public Headquarters getHeadquarters() { return headquarters; }

    /**
     * Sells a ticket based on a filled-out form.
     */
    public Ticket sellTicketFromForm(TicketForm form, long payment) {
        assert form != null : "Form cannot be null.";
        long cost = form.countValidBets() * form.getNumberOfDraws() * 300L;
        if (cost > payment) {
            return null; // Insufficient funds
        }

        ArrayList<int[]> bets = new ArrayList<>();
        for (int i = 1; i < 9; i++) {
            if (form.isValidBet(i)) {
                bets.add(form.getBet(i));
            }
        }

        Ticket newTicket = new Ticket(this, headquarters.getNextTicketNumber(), bets,
                form.getNumberOfDraws(), headquarters.getConductedDrawsCount() + 1);
        
        processSale(newTicket);
        return newTicket;
    }

    /**
     * Sells a "Quick Pick" ticket (random numbers).
     */
    public Ticket sellQuickPick(int betCount, int drawCount, long payment) {
        assert betCount > 0 && betCount < 9 : "Bet count must be 1-8.";
        assert drawCount > 0 && drawCount <= 10 : "Draw count must be 1-10.";
        if (betCount * drawCount * 300L > payment) {
            return null;
        }

        ArrayList<int[]> bets = new ArrayList<>();
        Draw randomGen = new Draw(0);
        for (int i = 0; i < betCount; i++) {
            bets.add(randomGen.getDrawnNumbers());
            randomGen = new Draw(0); // Refresh random
        }
        // Redundant getDrawnNumbers call removed from original code logic

        Ticket newTicket = new Ticket(this, headquarters.getNextTicketNumber(), bets,
                drawCount, headquarters.getConductedDrawsCount() + 1);

        processSale(newTicket);
        return newTicket;
    }

    private void processSale(Ticket ticket) {
        this.headquarters.addTicket(ticket);
        this.soldTickets.put(ticket, false);
        this.headquarters.transferTicketFees(ticket);
    }

    /**
     * Redeems a winning ticket for a player.
     * Calculates tax on high winnings and updates central funds.
     */
    public long payOutPrize(Ticket ticket, players.Player winner) {
        assert ticket != null && winner != null;
        if (!soldTickets.containsKey(ticket) || soldTickets.get(ticket)) {
            return 0; // Not sold here or already redeemed
        }

        long totalPrize = 0;
        long highWinTax = 0;

        for (int i = ticket.getDrawNumbers().getFirst(); i <= ticket.getDrawNumbers().getLast(); i++) {
            if (i > headquarters.getConductedDrawsCount()) break;

            int[] hits = ticket.checkHits(this.headquarters, i);
            long[] winAmounts = this.headquarters.getWinAmounts(i);

            for (int j = 0; j < hits.length; j++) {
                if (winAmounts[j] >= 228000) {
                    highWinTax += (winAmounts[j] / 10) * hits[j]; // 10% tax on high wins
                }
                totalPrize += hits[j] * winAmounts[j];
            }

            headquarters.updateFundsAfterPayout(i, totalPrize);
        }

        totalPrize -= highWinTax;
        this.headquarters.payTaxToBudget(highWinTax);

        winner.returnTicket(ticket);
        headquarters.removeTicket(ticket);
        soldTickets.put(ticket, true); // Mark as redeemed

        return totalPrize;
    }
}