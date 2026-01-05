package players;

import institutions.LotteryOutlet;
import institutions.Ticket;
import static java.lang.Math.min;

/**
 * A player who buys the minimum amount (1 bet, 1 draw) at a favorite outlet.
 */
public class MinimalistPlayer extends Player {
    private final LotteryOutlet favoriteOutlet;

    public MinimalistPlayer(String firstName, String lastName, String personalId, LotteryOutlet favoriteOutlet) {
        super(firstName, lastName, personalId, 10000000L);
        this.favoriteOutlet = favoriteOutlet;
        this.headquarters = favoriteOutlet.getHeadquarters();
    }

    @Override
    public void buy() {
        long payment = min(300L, this.funds);
        Ticket newTicket = this.favoriteOutlet.sellQuickPick(1, 1, payment);
        if (newTicket != null) {
            this.funds -= newTicket.getPrice();
            tickets.add(newTicket);
        }
    }
}