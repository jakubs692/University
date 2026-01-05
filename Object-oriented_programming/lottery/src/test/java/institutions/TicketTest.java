package institutions;

import org.junit.jupiter.api.Test;
import java.util.ArrayList;
import static org.junit.jupiter.api.Assertions.*;

/**
 * Tests for the Ticket class creation and pricing logic.
 */
public class TicketTest {

    StateTreasury treasury = StateTreasury.getInstance();
    Headquarters headquarters = new Headquarters(treasury);
    LotteryOutlet outlet = new LotteryOutlet(1, headquarters);
    
    int[] bet1 = new int[] { 1, 2, 3, 4, 5, 6};
    int[] bet2 = new int[] { 7, 8, 9, 10, 11, 12};
    ArrayList<int[]> bets = new ArrayList<>();

    @Test
    public void testValidTicketCreation() {
        // Assuming we sell a ticket for the next draw (1)
        Ticket ticket = new Ticket(outlet, 1, bets, 1, 1);
        assertNotNull(ticket, "Ticket should not be null.");
    }

    @Test
    public void testPriceCalculation() {
        bets.add(bet1);
        bets.add(bet2);
        // 2 bets * 1 draw * 300 = 600
        Ticket ticket = new Ticket(outlet, 1, bets, 1, 1);
        assertEquals(600, ticket.getPrice(), "Price should be 600.");
    }

    @Test
    public void testTaxCalculation() {
        bets.add(bet1);
        bets.add(bet2);
        bets.add(bet2);
        // 3 bets * 1 draw * 60 = 180 tax
        Ticket ticket = new Ticket(outlet, 1, bets, 1, 1);
        assertEquals(180, ticket.getTax(), "Tax should be 180.");
    }
}