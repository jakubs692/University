package players;

import institutions.Headquarters;
import institutions.LotteryOutlet;
import institutions.Ticket;
import java.util.Random;
import static java.lang.Math.min;

/**
 * A player who plays randomly: random outlet, random number of tickets, random bets.
 */
public class RandomPlayer extends Player {

    public RandomPlayer(String firstName, String lastName, String personalId, Headquarters headquarters) {
        super(firstName, lastName, personalId, new Random().nextLong(1000000000) + 1, headquarters);
    }

    @Override
    public void buy() {
        Random rand = new Random();
        int outletIdx = rand.nextInt(this.headquarters.getOutlets().size());
        LotteryOutlet selectedOutlet = this.headquarters.getOutlets().get(outletIdx);
        
        int ticketCount = rand.nextInt(100) + 1;

        for (int i = 0; i < ticketCount; i++) {
            int drawCount = rand.nextInt(10) + 1;
            int betCount = rand.nextInt(8) + 1;
            long payment = min(this.funds, betCount * drawCount * 300L);
            
            Ticket newTicket = selectedOutlet.sellQuickPick(betCount, drawCount, payment);
            if (newTicket != null) {
                this.tickets.add(newTicket);
                this.funds -= newTicket.getPrice();
            }
        }
    }
}