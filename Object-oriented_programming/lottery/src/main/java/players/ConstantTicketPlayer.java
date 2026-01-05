package players;

import institutions.*;
import java.util.ArrayList;
import static java.lang.Math.min;

/**
 * A player who uses the same filled-out form repeatedly.
 */
public class ConstantTicketPlayer extends Player {
    private final TicketForm favoriteForm;
    private final int drawInterval;
    private final ArrayList<LotteryOutlet> favoriteOutlets;
    private int nextOutletIndex;

    public ConstantTicketPlayer(String firstName, String lastName, String personalId, 
                                TicketForm favoriteForm, int drawInterval,
                                ArrayList<LotteryOutlet> favoriteOutlets, Headquarters headquarters) {
        super(firstName, lastName, personalId, 100000000L, headquarters);
        this.favoriteForm = favoriteForm;
        this.drawInterval = drawInterval;
        this.favoriteOutlets = favoriteOutlets;
        this.nextOutletIndex = 0;
    }

    @Override
    public void buy() {
        if (!tickets.isEmpty()) {
            if (tickets.getLast().getDrawNumbers().isEmpty()) return;
            int currentDraw = headquarters.getConductedDrawsCount();
            int lastDrawStart = tickets.getLast().getDrawNumbers().getFirst();
            
            if (currentDraw - lastDrawStart < drawInterval) {
                return;
            }
        }

        long cost = favoriteForm.countValidBets() * favoriteForm.getNumberOfDraws() * 300L;
        long payment = min(this.funds, cost);
        
        LotteryOutlet outlet = favoriteOutlets.get(nextOutletIndex);
        Ticket newTicket = outlet.sellTicketFromForm(favoriteForm, payment);
        
        if (newTicket != null) {
            this.funds -= newTicket.getPrice();
            this.tickets.add(newTicket);
        }
        
        nextOutletIndex = (nextOutletIndex + 1) % favoriteOutlets.size();
    }
}