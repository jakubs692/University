package institutions;

import players.*;
import org.junit.jupiter.api.Test;

import java.util.ArrayList;

import static java.lang.Math.max;
import static org.junit.jupiter.api.Assertions.*;

/**
 * Integration tests for the Headquarters and overall system flow.
 */
public class HeadquartersTest {

    StateTreasury treasury = StateTreasury.getInstance();
    // Re-create headquarters for each test to ensure clean state implies careful management,
    // but here we just instantiate it.
    Headquarters headquarters = new Headquarters(treasury);

    @Test
    public void testBudgetRevenue(){
        headquarters.addOutlet();
        Player[] players = new Player[100];
        LotteryOutlet outlet = headquarters.getOutlets().getFirst();

        for(int i = 0; i < 100; i++){
            // MinimalistPlayer buys 1 ticket (1 bet, 1 draw) -> Price 300, Tax 60
            Player player = new MinimalistPlayer("John", "Doe", "12345678901", outlet);
            players[i] = player;
        }

        long initialTax = treasury.getCollectedTaxes();
        
        for(Player player : players){
            player.buy();
        }
        
        // 100 players * 60 tax = 6000
        assertEquals(initialTax + 6000, treasury.getCollectedTaxes());
    }

    @Test
    public void testHeadquartersRevenue(){
        headquarters.addOutlet();
        Player[] players = new Player[100];
        LotteryOutlet outlet = headquarters.getOutlets().getFirst();

        for(int i = 0; i < 100; i++){
            Player player = new MinimalistPlayer("Jane", "Doe", "12345678902", outlet);
            players[i] = player;
        }

        // Draw #1 is the target for these tickets
        long initialFunds = 0; // Assuming fresh start for Draw 1 checks
        // Note: Headquarters initializes funds to 0.
        
        for(Player player : players){
            player.buy();
        }
    }

    @Test
    public void testPlayerFundsDecrease(){
        headquarters.addOutlet();
        Player[] players = new Player[100];
        LotteryOutlet outlet = headquarters.getOutlets().getFirst();

        for(int i = 0; i < 100; i++){
            Player player = new MinimalistPlayer("Alice", "Smith", "98765432100", outlet);
            players[i] = player;
        }

        for(Player player : players){
            player.buy();
        }
        
        for(Player player : players){
            // Minimalist starts with 10,000,000 (100k PLN). Ticket costs 300.
            assertEquals(10000000L - 300L, player.getFunds());
        }
    }

    @Test
    public void testFundsDistribution(){
        // Create a new scope for this test
        Headquarters hq = new Headquarters(treasury);
        hq.addOutlet();
        hq.addOutlet();
        ArrayList<Player> players = new ArrayList<>();

        for(int i = 0; i < 200; i++) {
            // RandomPlayer constructor: First, Last, ID, Headquarters
            RandomPlayer player = new RandomPlayer("Bob", "Builder", "55555555555", hq);
            players.add(player);
        }

        for (int i = 0; i < 20; i++) {
            for (Player player : players) {
                player.buy();
            }
            hq.conductDraw();
        }

        // Analyzing Draw #2
        DrawRecord record = null;
        long[] winAmounts = hq.getWinAmounts(2);
    }
}