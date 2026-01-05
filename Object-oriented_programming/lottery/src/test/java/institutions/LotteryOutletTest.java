package institutions;

import org.junit.jupiter.api.Test;
import java.util.ArrayList;
import static org.junit.jupiter.api.Assertions.*;

/**
 * Tests for LotteryOutlet functionality, including validation logic.
 */
class LotteryOutletTest {

    StateTreasury treasury = StateTreasury.getInstance();
    Headquarters headquarters = new Headquarters(treasury);

    @Test
    void testSellFromFormInsufficientFunds() {
        headquarters.addOutlet();
        LotteryOutlet favoriteOutlet = headquarters.getOutlets().getFirst();
        
        TicketForm form = new TicketForm();
        // Mark 6 numbers on field 1
        form.markNumber(1, 1); form.markNumber(1, 2);
        form.markNumber(1, 3); form.markNumber(1, 4);
        form.markNumber(1, 5); form.markNumber(1, 6);
        
        long insufficientPayment = 10L; 
        Ticket ticket = favoriteOutlet.sellTicketFromForm(form, insufficientPayment);
        assertNull(ticket, "Ticket should be null if payment is insufficient.");
    }

    @Test
    void testInvalidArgumentDraws() {
        LotteryOutlet testOutlet = new LotteryOutlet(1, headquarters);
        int invalidDraws = 20; // Max is 10

        AssertionError error = assertThrows(AssertionError.class, () -> {
            testOutlet.sellQuickPick(2, invalidDraws, 20000L);
        });

        assertTrue(error.getMessage().contains("Draw count must be 1-10"), 
                "Error message should mention invalid draw count. Got: " + error.getMessage());
    }

    @Test
    void testInvalidArgumentBets() {
        LotteryOutlet testOutlet = new LotteryOutlet(1, headquarters);
        int invalidBets = -2; // Must be 1-8
        
        AssertionError error = assertThrows(AssertionError.class, () -> {
            testOutlet.sellQuickPick(invalidBets, 2, 20000L);
        });

        assertTrue(error.getMessage().contains("Bet count must be 1-8"),
                "Error message should mention invalid bet count. Got: " + error.getMessage());
    }
}