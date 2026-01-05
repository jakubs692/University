package players;

import institutions.*;
import java.util.ArrayList;
import static java.lang.Math.min;

/**
 * A player who always plays the same set of numbers.
 */
public class ConstantNumbersPlayer extends Player {
    private final int[] favoriteNumbers;
    private final ArrayList<LotteryOutlet> favoriteOutlets;
    private int nextOutletIndex;

    public ConstantNumbersPlayer(String firstName, String lastName, String personalId, 
                                 int[] favoriteNumbers, int drawInterval,
                                 ArrayList<LotteryOutlet> favoriteOutlets, Headquarters headquarters) {
        super(firstName, lastName, personalId, 100000000L, headquarters);
        this.favoriteNumbers = favoriteNumbers;
        this.favoriteOutlets = favoriteOutlets;
        this.nextOutletIndex = 0;
    }

    @Override
    public void buy() {
        if (!tickets.isEmpty()) {
            Ticket last = tickets.getLast();
            int currentDraw = headquarters.getConductedDrawsCount();
            if (last.getDrawNumbers().isEmpty() || 
               (currentDraw - last.getDrawNumbers().getFirst() < 9)) {
                return;
            }
        }

        TicketForm form = new TicketForm();
        for (int num : favoriteNumbers) {
            form.markNumber(1, num); // Always bet 1
        }
        form.markNumberOfDraws(10);

        long cost = form.countValidBets() * form.getNumberOfDraws() * 300L;
        long payment = min(this.funds, cost);
        
        LotteryOutlet outlet = favoriteOutlets.get(nextOutletIndex);
        Ticket newTicket = outlet.sellTicketFromForm(form, payment);
        
        if (newTicket != null) {
            this.funds -= newTicket.getPrice();
            this.tickets.add(newTicket);
        }
        
        nextOutletIndex = (nextOutletIndex + 1) % favoriteOutlets.size();
    }
}