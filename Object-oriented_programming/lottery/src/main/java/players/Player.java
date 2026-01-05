package players;

import institutions.Headquarters;
import institutions.LotteryOutlet;
import institutions.Ticket;
import java.util.ArrayList;
import java.util.List;

/**
 * Abstract base class for all players.
 */
public abstract class Player {
    protected String firstName;
    protected String lastName;
    protected String personalId; // PESEL
    protected long funds;
    protected List<Ticket> tickets;
    protected Headquarters headquarters;

    protected Player(String firstName, String lastName, String personalId, long funds, Headquarters headquarters) {
        this.firstName = firstName;
        this.lastName = lastName;
        this.personalId = personalId;
        this.funds = funds;
        this.headquarters = headquarters;
        this.tickets = new ArrayList<>();
    }

    protected Player(String firstName, String lastName, String personalId, long funds) {
        this(firstName, lastName, personalId, funds, null);
    }

    public void returnTicket(Ticket ticket) {
        this.tickets.remove(ticket);
    }

    public void claimPrize(LotteryOutlet outlet, Ticket ticket) {
        long prize = outlet.payOutPrize(ticket, this);
        funds += prize;
    }

    public long getFunds() { return funds; }

    /**
     * Checks all owned tickets against the latest draw results.
     */
    public void checkResults() {
        if (tickets == null || headquarters == null) return;
        List<Ticket> toCheck = new ArrayList<>(this.tickets);
        for (Ticket ticket : toCheck) {
            if (ticket.getDrawNumbers().getLast() == headquarters.getConductedDrawsCount()) {
                claimPrize(ticket.getIssuingOutlet(), ticket);
            }
        }
    }

    public abstract void buy();

    @Override
    public String toString() {
        return String.format("Player %s %s (ID: %s) Funds: %.2f PLN",
                firstName, lastName, personalId, funds / 100.0);
    }
}